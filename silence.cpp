#include <windows.h>
#include "discord_rpc.h"
#include <time.h>

HANDLE g_ThreadHandle = NULL;
bool g_Running = true;

static void HandleDiscordReady(const DiscordUser* request) {
    // nada
}

DWORD WINAPI DiscordThread(LPVOID lpParam) {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = HandleDiscordReady;

    Discord_Initialize("1347921816777392244", &handlers, 1, NULL);

    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));
    presence.state = "";
    presence.details = "";
    presence.startTimestamp = time(0);
    presence.largeImageKey = "mobaxterm-logo";

    Discord_UpdatePresence(&presence);

    while (g_Running) {
        Discord_RunCallbacks();
        Sleep(500);
    }

    Discord_Shutdown();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
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

extern "C" __declspec(dllexport) void StartDiscordRPC() {
    if (g_ThreadHandle == NULL) {
        g_Running = true;
        g_ThreadHandle = CreateThread(NULL, 0, DiscordThread, NULL, 0, NULL);
    }
} 
