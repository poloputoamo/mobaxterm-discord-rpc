#include <windows.h>
#include "discord_rpc.h"
#include <iostream>
#include <time.h>
#include <tlhelp32.h>
#include <psapi.h>

HANDLE g_ThreadHandle = NULL;
bool g_Running = true;
time_t g_StartTimestamp = 0;

struct MobaState {
    bool isEditing;
    bool isTerminalActive;
    bool isLocalBash;
    HWND activeWindow;
} currentState;

bool IsProcessRunning(const wchar_t* processName) {
    static HANDLE snapshot = INVALID_HANDLE_VALUE;
    static DWORD lastCheck = 0;
    DWORD currentTime = GetTickCount();
    
    // updatear snapshot cada 500ms si peta, cambiarse
    if (currentTime - lastCheck > 500 || snapshot == INVALID_HANDLE_VALUE) {
        if (snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        lastCheck = currentTime;
    }

    if (snapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);

    if (!Process32FirstW(snapshot, &pe32)) return false;

    do {
        if (_wcsicmp(pe32.szExeFile, processName) == 0) {
            return true;
        }
    } while (Process32NextW(snapshot, &pe32));

    return false;
}

bool bashrun() {
    return IsProcessRunning(L"bash.exe") && IsProcessRunning(L"MoTTY.exe");
}

void UpdateMobaState() {
    currentState.isEditing = IsProcessRunning(L"MobaRTE.exe");
    currentState.isTerminalActive = IsProcessRunning(L"MoTTY.exe");
    currentState.isLocalBash = bashrun();
}

static void HandleDiscordReady(const DiscordUser* request) {
    g_StartTimestamp = time(0);
}

void UpdateDiscordPresence() {
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));
    
    // el main
    presence.startTimestamp = g_StartTimestamp;
    presence.largeImageKey = "moba-logo";
    presence.largeImageText = "MobaXterm";
    
    if (currentState.isEditing) {
        presence.state = "Editando archivos";
        presence.details = "Usando el editor de texto";
        presence.smallImageKey = "moba-logo";
        presence.smallImageText = "Editor";
    }
    else if (currentState.isLocalBash) {
        presence.state = "Terminal local";
        presence.details = "Bash";
        presence.smallImageKey = "bash";
        presence.smallImageText = "Local Bash";
    }
    else if (currentState.isTerminalActive) {
        presence.state = "Terminal activa";
        presence.details = "Terminal";
        presence.smallImageKey = "terminal";
        presence.smallImageText = "Terminal";
    }
    else {
        presence.state = "MobaXterm";
        presence.details = "Inactivo";
    }

    Discord_UpdatePresence(&presence);
}

DWORD WINAPI DiscordThread(LPVOID lpParam) {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = HandleDiscordReady;

    Discord_Initialize("1347921816777392244", &handlers, 1, NULL);
    g_StartTimestamp = time(0);

    while (g_Running) {
        UpdateMobaState();
        UpdateDiscordPresence();
        Discord_RunCallbacks();
        Sleep(500); // updatear cada 500ms, mmm puede que no sea buena idea, cambiar si peta todo
    }

    Discord_Shutdown();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        memset(&currentState, 0, sizeof(currentState));
        g_ThreadHandle = CreateThread(NULL, 0, DiscordThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        if (g_ThreadHandle != NULL) {
            g_Running = false;
            WaitForSingleObject(g_ThreadHandle, 1000);
            CloseHandle(g_ThreadHandle);
        }
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void StartDiscordRPC()
{
    if (g_ThreadHandle == NULL) {
        g_Running = true;
        g_ThreadHandle = CreateThread(NULL, 0, DiscordThread, NULL, 0, NULL);
    }
}
