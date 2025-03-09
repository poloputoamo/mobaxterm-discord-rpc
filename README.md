# MobaXterm Discord RPC Integration

Add Discord Rich Presence functionality to MobaXterm.  
*Note: This requires modifying original MobaXterm files. Backup your installation first.*

## Version Types

### 1. `mobaxterm-complete`
- **Tracks**: 
  - Active MobaRTE text editor sessions
  - Open terminal sessions

### 2. `mobaxterm-silence` 
- **Tracks**:
  - Only basic program open/closed status

---

## Installation Guide

### Prerequisites
- Windows OS
- Administrator privileges
- [Latest Release Files](https://github.com/poloputoamo/mobaxterm-discord-rpc/releases)

### Steps:
1. **Download Required Files**
   - Get `MobaXterm.exe`, `final.dll` and `discord-rpc.dll` from Releases (mobaxterm-(silence,complete) and change the name to MobaXterm.exe), (and for final.dll, the same)

2. **Locate Installation Directory**
   ```cmd
   C:\Program Files (x86)\Mobatek\MobaXterm
   ```

3. **Replace Files**
   - Backup original `MobaXterm.exe`
   - Replace with downloaded `MobaXterm.exe`
   - Add `discord-rpc.dll` to the directory
   - Add `final.dll` to the directory

4. **Restart MobaXterm**
   - Launch as administrator to ensure RPC functionality

---

## Build Instructions (Advanced)

### Requirements
- Windows: CFF Explorer (Recommended)
- Linux: Debian-based system (for cross-compilation)
- [discord-rpc.dll](https://github.com/discord/discord-rpc) (Windows 32-bit)
- GitHub Actions CI/CD Script Reference

### Windows Build Steps:
1. **Prepare DLL**
   - Obtain pre-built `discord-rpc.dll` -> win32-dynamic\bin from [discord-rpc repo](https://github.com/discord/discord-rpc/releases)
   - Obtain pre-built dlls from [here](https://github.com/poloputoamo/mobaxterm-discord-rpc/actions)

2. **Modify Executable with CFF Explorer**
   - Open original `MobaXterm.exe`
   - Navigate to `Import Adder`
   - Add Module → Select `discord-rpc.dll`
   - Export Functions → Import by Name
   - Rebuild Import Table
   - Save modified executable

### Linux Cross-Compilation (via CI/CD):
read [this](https://github.com/poloputoamo/mobaxterm-discord-rpc/tree/main/.github/workflows) :)
