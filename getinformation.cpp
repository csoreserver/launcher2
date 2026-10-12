#include "getinformation.h"
#include "hookutils.h"
#include "obfuscate.h"

#include "HLSDK/common/interface.h"
#include "interface/IFileSystem.h"
#include <string>
#include <iostream>
#include <windows.h>

extern IFileSystem* g_pFileSystem;

std::string GetExecutableName();

CreateHook(__cdecl, void, AddResources, const char* fileName, int priority, const char* locale, int unk, int unk2)
{
    std::cout << "Loading: " << fileName << " Locale: " << locale << std::endl;
    return g_pfnAddResources(fileName, priority, locale, unk, unk2);
}

CreateHook(__cdecl, void, LoadLocale, const char* fileName, int priority, const char* locale, int unk)
{
    std::cout << "Loading Locale: " << fileName << " Locale: " << locale << std::endl;
    return g_pfnLoadLocale(fileName, priority, locale, unk);
}

void GetInformation_Run(
    HMODULE hEngineModule,
    HMODULE hFileSystemModule,
    const char* cmdLine,
    const char* szNewCommandParams)
{
    DWORD g_dwEngineBase = GetModuleBase((HMODULE)hEngineModule);
    DWORD g_dwEngineSize = GetModuleSize((HMODULE)hEngineModule);

    #define ADDRESOURCES_CSNZ_SIG AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x83\xEC\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x56\x57\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\xC7\x85")
    #define ADDRESOURCES_CSNZ_MASK AY_OBFUSCATE("xxxx?x????xx????xxx?x????xxxx?xxxxx?xx????xx")

    #define LOADLOCALE_CSNZ_SIG AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x83\xEC\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x0F\x57\xC0\xC7\x45\x00\x00\x00\x00\x00\x0F\x11\x45\x00\xC7\x45\x00\x00\x00\x00\x00\xC6\x45")
    #define LOADLOCALE_CSNZ_MASK AY_OBFUSCATE("xxxx?x????xx????xxx?x????xxxx?xxx?xx????xxxxx?????xxx?xx?????xx")

    #define IFILESYSTEM_GLOBAL_CNSZ_SIG AY_OBFUSCATE("\xA3\x00\x00\x00\x00\x85\xC0\x74\x00\xE8\x00\x00\x00\x00\xF7\xD8")
    #define IFILESYSTEM_GLOBAL_CNSZ_MASK AY_OBFUSCATE("x????xxx?x????xx")

    #define GET_FILETIMESTAMP_CSNZ_SIG AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x00\x00\x00\x00\x74\x00\x33\xC0")
    #define GET_FILETIMESTAMP_CSNZ_MASK AY_OBFUSCATE("xxxxx?xx?????x?xx")

    DWORD find = NULL;
    find = FindPattern((PCHAR)ADDRESOURCES_CSNZ_SIG, (PCHAR)ADDRESOURCES_CSNZ_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!find)
        MessageBox(NULL, "AddResources == NULL!!!", "Error", MB_OK);
    else
        InlineHook((void*)find, Hook_AddResources, (void*&)g_pfnAddResources);

    find = FindPattern((PCHAR)LOADLOCALE_CSNZ_SIG, (PCHAR)LOADLOCALE_CSNZ_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!find)
        MessageBox(NULL, "LoadLocale == NULL!!!", "Error", MB_OK);
    else
        InlineHook((void*)find, Hook_LoadLocale, (void*&)g_pfnLoadLocale);

    find = FindPattern((PCHAR)IFILESYSTEM_GLOBAL_CNSZ_SIG, (PCHAR)IFILESYSTEM_GLOBAL_CNSZ_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!find)
    {
        MessageBox(NULL, "IFileSystem == NULL!!!", "Error", MB_OK);
        return;
    }

    typedef DWORD(__cdecl* GetFileTimestamp)(const char* lpFileName);
    GetFileTimestamp pGetFileTimestamp = (GetFileTimestamp)FindPattern((PCHAR)GET_FILETIMESTAMP_CSNZ_SIG, (PCHAR)GET_FILETIMESTAMP_CSNZ_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!pGetFileTimestamp)
    {
        MessageBox(NULL, "GetFileTimestamp == NULL!!!", "Error", MB_OK);
        return;
    }

    #define HWDLL_COMMAND_LINE_SIG_CSNZ AY_OBFUSCATE("\xBA\x00\x00\x00\x00\x8B\x75\x00\x8B\xCE\x8B\x7D\x00\x89\x85")
    #define HWDLL_COMMAND_LINE_MASK_CSNZ AY_OBFUSCATE("x????xx?xxxx?xx")

    find = FindPattern((PCHAR)HWDLL_COMMAND_LINE_SIG_CSNZ, (PCHAR)HWDLL_COMMAND_LINE_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!find)
    {
        MessageBox(NULL, "hw.dll Command Line == NULL!!!", "Error", MB_OK);
        return;
    }

    char* g_OrigCmd = (char*)(*(DWORD*)(find + 1));

    char* src = (char*)cmdLine;
    char* dst = g_OrigCmd;

    // Preserve original command line.
    do
    {
        *dst++ = *src++;
    } while (src[-1] != '\0');

    char* argv[50] = { 0 };
    int argc = 0;
    char* p = g_OrigCmd;

    while (*p && argc < 50)
    {
        // Skip characters outside printable ASCII.
        while (*p && (*p <= 0x20 || *p > 0x7E))
            ++p;

        if (!*p)
            break;

        argv[argc++] = p;

        // Find end of token.
        while (*p > 0x20 && *p <= 0x7E)
            ++p;

        // Terminate token in-place.
        if (*p)
            *p++ = '\0';
    }

    #define LOADARGV_CSNZ_SIG AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\x33\xC9\x53\x56\x57\x8B\x7D")
    #define LOADARGV_CSNZ_MASK AY_OBFUSCATE("xxxxx?xxxxxxx")

    typedef char*(__cdecl* LoadArgv)(int argc, char** argv);
    LoadArgv pLoadArgv = (LoadArgv)FindPattern((PCHAR)LOADARGV_CSNZ_SIG, (PCHAR)LOADARGV_CSNZ_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!pLoadArgv)
    {
        MessageBox(NULL, "LoadArgv == NULL!!!", "Error", MB_OK);
        return;
    }

    pLoadArgv(argc, argv);

    #define LOADRESOURCES_CSNZ_SIG AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x8B\x45\x00\x8D\x4D\x00\x56\x57")
    #define LOADRESOURCES_CSNZ_MASK AY_OBFUSCATE("xxxxx?x????xxxx?xx?xx?xx")

    typedef void(__cdecl* LoadResources)();
    LoadResources pLoadResources = (LoadResources)FindPattern((PCHAR)LOADRESOURCES_CSNZ_SIG, (PCHAR)LOADRESOURCES_CSNZ_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
    if (!pLoadResources)
    {
        MessageBox(NULL, "LoadResources == NULL!!!", "Error", MB_OK);
        return;
    }

    g_pFileSystem->_LoadResourceNARByName("Path/Data/fixtrike.nar", "/fixtrike/");

    DWORD exeTimestamp = pGetFileTimestamp(GetExecutableName().c_str());
    DWORD hwTimestamp = pGetFileTimestamp("hw.dll") ^ exeTimestamp;
    DWORD mpTimestamp = pGetFileTimestamp("mp.dll") ^ hwTimestamp;
    DWORD clTimestamp = pGetFileTimestamp("client.dll") ^ mpTimestamp;
    DWORD gameuiTimestamp = pGetFileTimestamp("gameui.dll") ^ clTimestamp;
    DWORD vgui2Timestamp = pGetFileTimestamp("vgui2.dll") ^ gameuiTimestamp;
    DWORD fileTimestamp = pGetFileTimestamp("filesystem_nar.dll") ^ vgui2Timestamp;

    DWORD narcrc = g_pFileSystem->_GetNarCRC();

    std::cout << "[Type: DWORD] Game Build Timestamp: " << fileTimestamp << std::endl;
    std::cout << "[Type: DWORD] NAR CRC: " << narcrc << std::endl;

    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
}