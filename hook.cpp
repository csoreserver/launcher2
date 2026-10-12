// Sempak nih, yg bikin launcher nya bikin orang bikin sakit kepala

#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <random>

#include "hook.h"
#include "hookutils.h"
#include "ClassicUI.h"
#include <stdio.h>
#include <locale.h>
#include <ICommandLine.h>
#include <IGameUI.h>
#include <VGUI/IPanel.h>
#include "DediCsv.h"
#include "ChattingManager.h"
#include <IFileSystem.h>

#include <tlhelp32.h>
#include <iostream>
#include <thread>
#include <winsvc.h>
#include <array>
#include <string>
#include <algorithm>
#include <filesystem>

#define MAX_ZIP_SIZE (1024 * 1024 * 16)
#include "XZip.h"

#include <vector>
#include <string>
#include <unordered_map>
#include "sys.h"
#include "obfuscate.h"

extern IFileSystem* g_pFileSystem;

HMODULE g_hEngineModule;
DWORD g_dwEngineBase;
DWORD g_dwEngineSize;

DWORD g_dwGameUIBase;
DWORD g_dwGameUISize;

DWORD g_dwMpBase;
DWORD g_dwMpSize;

DWORD g_dwClientBase;
DWORD g_dwClientSize;

DWORD g_dwFileSystemBase;
DWORD g_dwFileSystemSize;

#define DEFAULT_IP AY_OBFUSCATE("127.0.0.1")
#define DEFAULT_PORT AY_OBFUSCATE("30002")
#define SOCKETMANAGER_SIG_CSNZ23 AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x51\x53\x56\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x8B\xD9\x89\x5D\x00\x8A\x45")
#define SOCKETMANAGER_MASK_CSNZ23 AY_OBFUSCATE("xxxx?x????xx????xxxxxx????xxxxx?xx????xxxx?xx")
#define SERVERCONNECT_SIG_CSNZ2019 AY_OBFUSCATE("\xE8\x00\x00\x00\x00\x85\xC0\x75\x00\x46")
#define SERVERCONNECT_MASK_CSNZ2019 AY_OBFUSCATE("x????xxx?x")
#define PACKET_METADATA_PARSE_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x56\x57\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x8B\xF1\x89\xB5\x00\x00\x00\x00\x8B\x45\x00\x89\x85")
#define PACKET_METADATA_PARSE_MASK_CSNZ AY_OBFUSCATE("xxxx?x????xx????xxx????x????xxxx?xxxxx?xx????xxxx????xx?xx")
#define PACKET_HACK_PARSE_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x83\xEC\x00\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x8B\xF9\x8B\x45\x00\x89\x45")
#define PACKET_HACK_PARSE_MASK_CSNZ AY_OBFUSCATE("xxxx?x????xx????xxx?xx????xxxxx?xx????xxxx?xx")
#define PACKET_HACK_SEND_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x3D\x00\x00\x00\x00\x00\x74\x00\xE8\x00\x00\x00\x00\xE8")
#define PACKET_HACK_SEND_MASK_CSNZ AY_OBFUSCATE("x????x????x????x????xx????xx?x????x")
#define BOT_MANAGER_PTR_SIG_CSNZ AY_OBFUSCATE("\xA3\x00\x00\x00\x00\xC7\x45\x00\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x83\xC4")
#define BOT_MANAGER_PTR_MASK_CSNZ AY_OBFUSCATE("x????xx?????xx????xx")
#define CSOMAINPANEL_PTR_SIG_CSNZ AY_OBFUSCATE("\x8B\x0D\x00\x00\x00\x00\x6A\x00\x00\x00\xFF\x90\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\xFF\x75")
#define CSOMAINPANEL_PTR_MASK_CSNZ AY_OBFUSCATE("xx????x???xx????xx????xx")
#define CALL_PANEL_FINDCHILDBYNAME_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\x85\xC0\x74\x00\x83\x7D")
#define CALL_PANEL_FINDCHILDBYNAME_MASK_CSNZ AY_OBFUSCATE("x????xxx?xx")
#define NGCLIENT_INIT_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\x83\xC4\x00\x85\xC0\x74\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0")
#define NGCLIENT_INIT_MASK_CSNZ AY_OBFUSCATE("x????xx?xxx?x????x????xx")
#define NGCLIENT_QUIT_SIG_CSNZ AY_OBFUSCATE("\x57\xE8\x00\x00\x00\x00\xFF\x15")
#define NGCLIENT_QUIT_MASK_CSNZ AY_OBFUSCATE("xx????xx")
#define HOLEPUNCH_SETSERVERINFO_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\xB8\x00\x00\x00\x00\x66\xA3")
#define HOLEPUNCH_SETSERVERINFO_MASK_CSNZ AY_OBFUSCATE("xxxx????xx")
#define HOLEPUNCH_GETUSERSOCKETINFO_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\x57\x8B\x7D\x00\x85\xFF\x75\x00\x8B\x45")
#define HOLEPUNCH_GETUSERSOCKETINFO_MASK_CSNZ AY_OBFUSCATE("xxxxx?xxx?xxx?xx")
#define CREATESTRINGTABLE_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x53\x56\x8B\xF1\xC7\x46")
#define CREATESTRINGTABLE_MASK_CSNZ AY_OBFUSCATE("xxxxxxxxx")
#define LOADJSON_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x53\x56\x8B\x75\x00\x8B\x01\x57\x8B\x50\x00\x8B\x45\x00\x83\x78\x00\x00\x76\x00\x8B\x00\x6A\x00\x68\x00\x00\x00\x00\x50\xFF\xD2\x8B\x0D\x00\x00\x00\x00\x8B\xD8\x53\x8B\x11\xFF\x52\x00\x8B\xF8\x85\xFF\x74")
#define LOADJSON_MASK_CSNZ AY_OBFUSCATE("xxxxx????xxxx?xxxxx?xx?xx??x?xxx?x????xxxxx????xxxxxxx?xxxxx")
#define LOGTOERRORLOG_SIG_CSNZ AY_OBFUSCATE("\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x00\x89\x6C\x24\x00\x8B\xEC\x83\xEC\x00\x56\x57\x8B\x7B\x00\x85\xFF")
#define LOGTOERRORLOG_MASK_CSNZ AY_OBFUSCATE("xxxxx?xx?xx?xxx?xxx?xxxx?xxxx?xx")
#define READPACKET_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\x8B\xF0\x83\xFE\x00\x77")
#define READPACKET_MASK_CSNZ AY_OBFUSCATE("x????xxxx?x")
#define GETSSLPROTOCOLNAME_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x8A\x10")
#define GETSSLPROTOCOLNAME_MASK_CSNZ AY_OBFUSCATE("x????x????xx")
#define SOCKETCONSTRUCTOR_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\xEB\x00\x33\xC0\xFF\x75\x00\xC7\x45")
#define SOCKETCONSTRUCTOR_MASK_CSNZ AY_OBFUSCATE("x????x?xxxx?xx")
#define ZBSELITESPAWN_SIG_CSNZ AY_OBFUSCATE("\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xFF\x83\x00\x01\x00\x00")
#define ZBSELITESPAWN_MASK_CSNZ AY_OBFUSCATE("x????x????xx?xxx")
#define EVP_CIPHER_CTX_NEW_SIG_CSNZ AY_OBFUSCATE("\xE8\x00\x00\x00\x00\x8B\xF8\x89\xBE")
#define EVP_CIPHER_CTX_NEW_MASK_CSNZ AY_OBFUSCATE("x????xxxx")
#define FPS_PATCH_SIG_CSNZ AY_OBFUSCATE("\x9F\xF6\xC4\x00\x7A\x00\xF2\x0F\x10\x0D\x00\x00\x00\x00\x0F\x5A\xC2\xF2\x0F\x5D\xC8\x66\x0F\x5A\xD1\x83\xFA\x00\x7E\x00\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x5F\xC2\x0F\x28\xD0\x83\x3D\x00\x00\x00\x00\x00\x75\x00\x0F\x28")
#define FPS_PATCH_MASK_CSNZ AY_OBFUSCATE("xxx?x?xxxx????xxxxxxxxxxxxx?x?xxxx????xxxxxxxxx????xx?xx")
#define GETPECOMPILATIONTIME_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x00\x00\x00\x00\x74\x00\x33\xC0")
#define GETPECOMPILATIONTIME_MASK_CSNZ AY_OBFUSCATE("xxxxx?xx????xx?xx")
#define CONNECTSERVER_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x83\x3D\x00\x00\x00\x00\x00\x53\x56\x8B\xD9")
#define CONNECTSERVER_MASK_CSNZ AY_OBFUSCATE("xxxxx????x????xxxx?xx????xxxxx")
#define CMAPENTITYMANAGER_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x51\xA1\x00\x00\x00\x00\x85\xC0\x75\x00\x6A\x00\xE8\x00\x00\x00\x00\x83\xC4\x00\x89\x45\x00\x85\xC0\x74\x00\x00\x00\x00\x00\x00\x00\x66\xC7\x40")
#define CMAPENTITYMANAGER_MASK_CSNZ AY_OBFUSCATE("xxxxx????xxx?x?x????xx?xx?xxx???????xxx")
#define CGOLDENZOMBIEMANAGER_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x53\x56\x57\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x8B\xF9\x89\xBD\x00\x00\x00\x00\x89\xBD")
#define CGOLDENZOMBIEMANAGER_MASK_CSNZ AY_OBFUSCATE("xxxx?x????xx????xx????x????x????xxxx?xxxxxx?xx????xxxx????xx")
#define SETGAMEEVENT_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\x80\x7D\x00\x00\x56\x8B\xF1\xC6\x86")
#define SETGAMEEVENT_MASK_CSNZ AY_OBFUSCATE("xxxxx?xx??xxxxx")
#define CGAMEUI_PANELPOS_SIG_CSNZ AY_OBFUSCATE("\x6A\x00\xE8\x00\x00\x00\x00\x8B\x03\x8B\xCB\x6A\x00\xFF\x50")
#define CGAMEUI_PANELPOS_MASK_CSNZ AY_OBFUSCATE("x?x????xxxxx?xx")
#define CGAMEUI_PANELPOS2_SIG_CSNZ AY_OBFUSCATE("\x6A\x00\xE8\x00\x00\x00\x00\x8B\x4D\x00\x5F")
#define CGAMEUI_PANELPOS2_MASK_CSNZ AY_OBFUSCATE("x?x????xx?x")
#define CSOWEBMDECODER_LOAD_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x83\xEC\x00\x53\x56\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x8B\xF1\xA1")
#define CSOWEBMDECODER_LOAD_MASK_CSNZ AY_OBFUSCATE("xxxx?x????xx????xxx?xxxx????xxxxx?xx????xxx")
#define GOLDENZOMBIEFORLAUNCHER_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x53\x56\x57\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x89\x65\x00\x83\x3D\x00\x00\x00\x00\x00\x8B\x7D\x00\x8B\x75")
#define GOLDENZOMBIEFORLAUNCHER_MASK_CSNZ AY_OBFUSCATE("xxxx?x????xx????xxx????x????xxxx?xxxxxx?xx????xx?xx?????xx?xx")
#define WRITEPACKET_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x51\x53\x56\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\x00\x64\xA3\x00\x00\x00\x00\x8B\xF1\x80\x7E")
#define WRITEPACKET_MASK_CSNZ AY_OBFUSCATE("xxxx?x????xx????xxxxxx????xxxxx?xx????xxxx")
#define CSONWRAPPER_LOGIN_SIG AY_OBFUSCATE("\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x83\x3D\x00\x00\x00\x00\x00\x53\x8B\x5D\x00\x56\x8B\x75\x00\x57\x8B\xF9")
#define CSONWRAPPER_LOGIN_MASK AY_OBFUSCATE("xxxxx????x????xxxx?xx?????xxx?xxx?xxx")
#define ITEM_TO_WEAPONINDEX_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x08\x53\x8B\x5D\x08\x0F\xB6\xC3\x35\xC5\x9D\x1C\x81\x69\xD0\x93\x01\x00\x01\x8B\xC3\xC1\xF8\x08\x0F\xB6\xC0\x56\x8B\xF1\x33\xD0\x8B\xC3\xC1\xF8\x10\x69\xCA\x93\x01\x00\x01\x0F\xB6\xC0\xC1\xFB\x18\x57\x33\xC8\x0F\xB6\xC3\x69\xC9\x93\x01\x00\x01\x33\xC8\x69\xC1\x93\x01\x00\x01\x8D\x8E\x88\x99\x01\x00\x50\x8D\x45\x08\x50\x8D\x45\xF8\x50\xE8\x00\x00\x00\x00\x8B\x40\x04\x85\xC0\x75\x06\x8B\x86\x8C\x99\x01\x00\x3B\x86\x8C\x99\x01\x00\x5F\x5E\x5B\x74\x09\x8B\x40\x0C\x8B\xE5\x5D\xC2\x04\x00")
#define ITEM_TO_WEAPONINDEX_MASK_CSNZ AY_OBFUSCATE("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
#define GAMEMODE_OBJECT_SIG_CSNZ AY_OBFUSCATE("\xB8\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xFF\x90\x38\x02\x00\x00\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xFF\x90\x88\x01\x00\x00")
#define GAMEMODE_OBJECT_MASK_CSNZ AY_OBFUSCATE("x????xxxxxxxxxxxx????x????xxxxxxx????x????xxxxxx")
#define BUYMENU_REFRESH_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x81\xEC\x90\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xFC\x8B\x45\x08\x53\x8B\xD9\x56\x8B\x75\x0C\x89\x5D\xA0\x3B\x83\x00\x00\x00\x00\x75\x08\x85\xF6\x0F\x84")
#define BUYMENU_REFRESH_MASK_CSNZ AY_OBFUSCATE("xxxxxxxxxx????xxxxxxxxxxxxxxxxxxxx????xxxxxx")
#define WEAPONINFO_LOOKUP_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x51\x53\x56\x57\x8B\xF1\xE8\x00\x00\x00\x00\x8B\x55\x08\x8B\xF8\x8D\x45\xFC\x8B\xCF\x89\x45\x08\x8D\x1C\x52\x8B\x15\x00\x00\x00\x00\xC1\xE3\x04")
#define WEAPONINFO_LOOKUP_MASK_CSNZ AY_OBFUSCATE("xxxxxxxxxx????xxxxxxxxxxxxxxxxxx????xxx")

float *g_pFreezeTime;

char g_pServerIP[16];
char g_pServerPort[16];
char g_pLogin[64];
char g_pPassword[64];

bool g_bUseOriginalServer = false;
bool g_bDumpMetadata = false;
bool g_bIgnoreMetadata = false;
bool g_bDumpAll = false;
bool g_bDisableAuthUI = false;
bool g_bUseSSL = false;
bool g_bWriteMetadata = false;
bool g_bLoadDediCsvFromFile = false;
bool g_bRegister = false;
bool g_bNoNGHook = false;
bool g_bNoZBSElitePatch = false;
bool g_bFixWeapon = false;
bool g_bGoldenZombie = false;
bool g_bClassicUI = false; // --classicui: apply the classic UI preset (ClassicUI.cpp) on allowedGameModes
bool g_bVersionOK = false;
bool g_bKickedFromServer = false;

cl_enginefunc_t *g_pEngine;

class CCSBotManager
{
public:
	virtual void Unknown() = NULL;
	virtual void Unknown2() = NULL;
	virtual void Bot_Add(int side) = NULL;
};

CCSBotManager *g_pBotManager = NULL;

class CGoldenZombieManager
{
public:
	int m_iMaxGoldenZombieSpawn;
	virtual ~CGoldenZombieManager() {};
};

CGoldenZombieManager *g_pGoldenZombieMgr = nullptr;
float *g_flZSCountDown = nullptr;

vgui::IPanel *g_pPanel = nullptr;
IGameUI *g_pGameUI = nullptr;
ChattingManager *g_pChattingManager;

WNDPROC oWndProc;
HWND hWnd;

int(__thiscall *g_pfnGameUI_RunFrame)(void *_this);

typedef void *(__thiscall *tPanel_FindChildByName)(void *_this, const char *name, bool recurseDown);
tPanel_FindChildByName g_pfnPanel_FindChildByName;

typedef int(__thiscall *tLoginDlg_OnCommand)(void *_this, const char *command);
tLoginDlg_OnCommand g_pfnLoginDlg_OnCommand;

typedef void(__thiscall *tParseCSV)(int *_this, unsigned char *buffer, int size);
tParseCSV g_pfnParseCSV;

typedef void *(*tEVP_CIPHER_CTX_new)();
tEVP_CIPHER_CTX_new g_pfnEVP_CIPHER_CTX_new;

char NGClient_Return1()
{
	return 1;
}

int NGClient_Return17238()
{
	return 17238;
}

// logger shit
bool NXGSM_Dummy()
{
	return false;
}

void NXGSM_WriteStageLogA(int a1, char *a2)
{
}

void NXGSM_WriteErrorLogA(int a1, char *a2)
{
}

void string_trim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
									{ return !std::isspace(ch); }));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
						 { return !std::isspace(ch); })
				.base(),
			s.end());
}

void string_to_lower(std::string &s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
				   { return std::tolower(c); });
}

__declspec(noinline) void Pbuf_AddText(const char* text)
{
	g_pEngine->pfnClientCmd((char*)text);
}

CreateHookClass(void *, SocketManagerConstructor, bool useSSL)
{
	return g_pfnSocketManagerConstructor(ptr, g_bUseSSL);
}

CreateHookClass(int, ServerConnect, unsigned long ip, unsigned short port, bool validate)
{
	return g_pfnServerConnect(ptr, inet_addr(g_pServerIP), htons(atoi(g_pServerPort)), validate);
}

CreateHook(__cdecl, void, HolePunch_SetServerInfo, unsigned long ip, unsigned short port)
{
	g_pfnHolePunch_SetServerInfo(inet_addr(g_pServerIP), htons(atoi(g_pServerPort)));
}

CreateHookClass(void *, GoldenZombieInit)
{
	void *_this = g_pfnGoldenZombieInit(ptr);
	if (!g_pGoldenZombieMgr)
		g_pGoldenZombieMgr = (CGoldenZombieManager *)ptr;
	return _this;
}

CreateHookClass(void *, SetGameEvent, int i)
{
	void *_this = g_pfnSetGameEvent(ptr, i);

	const char *maxGoldenZombie;
	if (CommandLine()->CheckParm("-maxgoldenzombie", &maxGoldenZombie) != NULL)
	{
		g_pGoldenZombieMgr->m_iMaxGoldenZombieSpawn = atoi(maxGoldenZombie);
	}
	return _this;
}

CreateHookClass(void, ZSStartCountDown)
{
	*g_flZSCountDown = 20;
	g_pfnZSStartCountDown(ptr);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == 0x113 && wParam == 250)
	{
		return 0;
	}
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool InjectDLL(DWORD processID, const std::string &dllPath)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	if (!hProcess)
		return false;

	LPVOID allocMem = VirtualAllocEx(hProcess, NULL, dllPath.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!allocMem)
		return false;

	WriteProcessMemory(hProcess, allocMem, dllPath.c_str(), dllPath.size() + 1, NULL);

	HMODULE hKernel32 = GetModuleHandleA("Kernel32");
	FARPROC loadLibAddr = GetProcAddress(hKernel32, "LoadLibraryA");

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, allocMem, 0, NULL);
	if (!hThread)
		return false;

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return true;
}

enum dediCsvType
{
	TDM_Spawn_Replacement,
	AllStar_Skill,
	AllStar_Status,
	LastStand,
	ProtectionSupplyWeapon,
	RandomRule_Classic,
	RandomRule,
	ZSRogueLiteAbility,
	ZSTransform_Skill,
	ZSTransform_Status,
	FireBombOption,
	ZombieSkillProperty_Crazy,
	ZombieSkillProperty_JumpBuff,
	ZombieSkillProperty_ArmorUp,
	ZombieSkillProperty_Heal,
	ZombieSkillProperty_ShieldBuf,
	ZombieSkillProperty_Cloacking,
	ZombieSkillProperty_Trap,
	ZombieSkillProperty_Smoke,
	ZombieSkillProperty_VoodooHeal,
	ZombieSkillProperty_Shock,
	ZombieSkillProperty_Rush,
	ZombieSkillProperty_Pile,
	ZombieSkillProperty_Bat,
	ZombieSkillProperty_Stiffen,
	ZombieSkillProperty_SelfDestruct,
	ZombieSkillProperty_Penetration,
	ZombieSkillProperty_Revival,
	ZombieSkillProperty_Telleport,
	ZombieSkillProperty_Boost,
	ZombieSkillProperty_BombCreate,
	ZombieSkillProperty_Flying,
	ZombieSkillProperty_Fireball,
	ZombieSkillProperty_DogShoot,
	ZombieSkillProperty_ViolentRush,
	ZombieSkillProperty_WebShooter,
	ZombieSkillProperty_WebBomb,
	ZombieSkillProperty_Protect,
	ZombieSkillProperty_ChargeSlash,
	ZombieSkillProperty_Claw,
	HumanAbilityData,
	HumanAbilityProbData,
	SpecialZombieProb,
	VirusFactorReq,
	ZombiVirusBonus,
	ZombieSkillProperty_Confuse
};

std::unordered_map<std::string, dediCsvType> dediCsv = {
	{("fixtrike/maps/TDM_Spawn_Replacement_Dedi.csv"), TDM_Spawn_Replacement},
	{("fixtrike/resource/allstar/AllStar_Skill-Dedi.csv"), AllStar_Skill},
	{("fixtrike/resource/allstar/AllStar_Status-Dedi.csv"), AllStar_Status},
	{("fixtrike/resource/ModeEvent/LastStand_Dedi.csv"), LastStand},
	{("fixtrike/resource/ModeEvent/ProtectionSupplyWeapon_Dedi.csv"), ProtectionSupplyWeapon},
	{("fixtrike/resource/ModeEvent/RandomRule_Classic_Dedi.csv"), RandomRule_Classic},
	{("fixtrike/resource/ModeEvent/RandomRule_Dedi.csv"), RandomRule},
	{("fixtrike/resource/ModeEvent/ZSRogueLiteAbility_Dedi.csv"), ZSRogueLiteAbility},
	{("fixtrike/resource/ModeEvent/ZSTransform_Skill-Dedi.csv"), ZSTransform_Skill},
	{("fixtrike/resource/ModeEvent/ZSTransform_Status-Dedi.csv"), ZSTransform_Status},
	{("fixtrike/resource/zombi/FireBombOption_Dedi.csv"), FireBombOption},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Crazy.csv"), ZombieSkillProperty_Crazy},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_JumpBuff.csv"), ZombieSkillProperty_JumpBuff},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_ArmorUp.csv"), ZombieSkillProperty_ArmorUp},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Heal.csv"), ZombieSkillProperty_Heal},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_ShieldBuf.csv"), ZombieSkillProperty_ShieldBuf},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Cloacking.csv"), ZombieSkillProperty_Cloacking},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Trap.csv"), ZombieSkillProperty_Trap},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Smoke.csv"), ZombieSkillProperty_Smoke},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_VoodooHeal.csv"), ZombieSkillProperty_VoodooHeal},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Shock.csv"), ZombieSkillProperty_Shock},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Rush.csv"), ZombieSkillProperty_Rush},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Pile.csv"), ZombieSkillProperty_Pile},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Bat.csv"), ZombieSkillProperty_Bat},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Stiffen.csv"), ZombieSkillProperty_Stiffen},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_SelfDestruct.csv"), ZombieSkillProperty_SelfDestruct},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Penetration.csv"), ZombieSkillProperty_Penetration},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Revival.csv"), ZombieSkillProperty_Revival},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Telleport.csv"), ZombieSkillProperty_Telleport},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Boost.csv"), ZombieSkillProperty_Boost},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_BombCreate.csv"), ZombieSkillProperty_BombCreate},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Flying.csv"), ZombieSkillProperty_Flying},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Fireball.csv"), ZombieSkillProperty_Fireball},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_DogShoot.csv"), ZombieSkillProperty_DogShoot},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_ViolentRush.csv"), ZombieSkillProperty_ViolentRush},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_WebShooter.csv"), ZombieSkillProperty_WebShooter},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_WebBomb.csv"), ZombieSkillProperty_WebBomb},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Protect.csv"), ZombieSkillProperty_Protect},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_ChargeSlash.csv"), ZombieSkillProperty_ChargeSlash},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Claw.csv"), ZombieSkillProperty_Claw},
	{("fixtrike/resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Confuse.csv"), ZombieSkillProperty_Confuse},
	{("fixtrike/resource/zombi5/HumanAbilityData_Dedi.csv"), HumanAbilityData},
	{("fixtrike/resource/zombi5/HumanAbilityProbData_Dedi.csv"), HumanAbilityProbData},
	{("fixtrike/resource/zombi5/SpecialZombieProb_Dedi.csv"), SpecialZombieProb},
	{("fixtrike/resource/zombi5/VirusFactorReq_Dedi.csv"), VirusFactorReq},
	{("fixtrike/resource/zombi5/ZombiVirusBonus_Dedi.csv"), ZombiVirusBonus},
};

bool LoadCsv(int *_this, const char *filename, unsigned char *defaultBuf, int defaultBufSize)
{
	unsigned char *buffer = NULL;
	long size = 0;

	if (g_bLoadDediCsvFromFile)
	{
		char path[MAX_PATH];
		snprintf(path, sizeof(path), AY_OBFUSCATE("%s/Data/%s"), Sys_GetLongPathNameWithoutBin(), filename);

		FILE *file = fopen(path, "rb");
		if (!file)
		{
			printf(AY_OBFUSCATE("LoadCsv: %s failed to load from file (file == NULL), loading from filesystem\n"), filename);
			goto LoadFileSystem;
		}

		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		if (size)
		{
			buffer = (unsigned char *)malloc(size);
			if (buffer)
				fread(buffer, 1, size, file);
			else
				printf(AY_OBFUSCATE("LoadCsv: %s failed to load from file (malloc failed), loading from filesystem\n"), filename);
		}
		else
			printf(AY_OBFUSCATE("LoadCsv: %s failed to load from file (size <= 0), loading from filesystem\n"), filename);

		fclose(file);

		if (buffer)
			goto SetBuffer;
	}

LoadFileSystem:
	FileHandle_t fh = g_pFileSystem->Open(filename, AY_OBFUSCATE("rb"), 0);
	if (!fh)
	{
		printf(AY_OBFUSCATE("LoadCsv: %s failed to load from filesystem (fh == NULL), loading hardcoded values\n"), filename);
		goto LoadDefaultBuf;
	}

	size = g_pFileSystem->Size(fh);
	if (size)
	{
		buffer = (unsigned char *)malloc(size);
		if (buffer)
			g_pFileSystem->Read(buffer, size, fh);
		else
			printf(AY_OBFUSCATE("LoadCsv: %s failed to load from filesystem (malloc failed), loading hardcoded values\n"), filename);
	}
	else
		printf(AY_OBFUSCATE("LoadCsv: %s failed to load from filesystem (size <= 0), loading hardcoded values\n"), filename);

	g_pFileSystem->Close(fh);

	if (buffer)
		goto SetBuffer;

LoadDefaultBuf:
	buffer = defaultBuf;
	size = defaultBufSize;

SetBuffer:
	g_pfnParseCSV(_this, buffer, size);

	bool result = 0;
	if (_this[2])
		result = _this[3] != 0;

	return result;
}

CreateHookClassType(bool, CreateStringTable, int, const char *filename)
{
	std::string filenameStr = filename;
	if (filenameStr.find("maps/BoostingPoints_Dedi_") != std::string::npos)
		return LoadCsv(ptr, filename, NULL, NULL);

	if (dediCsv.find(filename) != dediCsv.end())
	{
		switch (dediCsv[filename])
		{
		case TDM_Spawn_Replacement:
			return LoadCsv(ptr, filename, g_TDM_Spawn_Replacement, sizeof(g_TDM_Spawn_Replacement));
		case AllStar_Skill:
			return LoadCsv(ptr, filename, g_AllStar_Skill, sizeof(g_AllStar_Skill));
		case AllStar_Status:
			return LoadCsv(ptr, filename, g_AllStar_Status, sizeof(g_AllStar_Status));
		case LastStand:
			return LoadCsv(ptr, filename, g_LastStand, sizeof(g_LastStand));
		case ProtectionSupplyWeapon:
			return LoadCsv(ptr, filename, g_ProtectionSupplyWeapon, sizeof(g_ProtectionSupplyWeapon));
		case RandomRule_Classic:
			return LoadCsv(ptr, filename, g_RandomRule_Classic, sizeof(g_RandomRule_Classic));
		case RandomRule:
			return LoadCsv(ptr, filename, g_RandomRule, sizeof(g_RandomRule));
		case ZSRogueLiteAbility:
			return LoadCsv(ptr, filename, g_ZSRogueLiteAbility, sizeof(g_ZSRogueLiteAbility));
		case ZSTransform_Skill:
			return LoadCsv(ptr, filename, g_ZSTransform_Skill, sizeof(g_ZSTransform_Skill));
		case ZSTransform_Status:
			return LoadCsv(ptr, filename, g_ZSTransform_Status, sizeof(g_ZSTransform_Status));
		case FireBombOption:
			return LoadCsv(ptr, filename, g_FireBombOption, sizeof(g_FireBombOption));
		case HumanAbilityData:
			return LoadCsv(ptr, filename, g_HumanAbilityData, sizeof(g_HumanAbilityData));
		case HumanAbilityProbData:
			return LoadCsv(ptr, filename, g_HumanAbilityProbData, sizeof(g_HumanAbilityProbData));
		case SpecialZombieProb:
			return LoadCsv(ptr, filename, g_SpecialZombieProb, sizeof(g_SpecialZombieProb));
		case VirusFactorReq:
			return LoadCsv(ptr, filename, g_VirusFactorReq, sizeof(g_VirusFactorReq));
		case ZombiVirusBonus:
			return LoadCsv(ptr, filename, g_ZombiVirusBonus, sizeof(g_ZombiVirusBonus));
		}
	}

	return g_pfnCreateStringTable(ptr, filename);
}

bool LoadJson(std::string *filename, std::string *oriBuf, unsigned char *defaultBuf, int defaultBufSize)
{
	unsigned char *buffer = NULL;
	long size = 0;

	if (g_bLoadDediCsvFromFile)
	{
		char path[MAX_PATH];
		snprintf(path, sizeof(path), AY_OBFUSCATE("%s/Data/%s"), Sys_GetLongPathNameWithoutBin(), filename->c_str());

		FILE *file = fopen(path, "rb");
		if (!file)
		{
			printf(AY_OBFUSCATE("LoadJson: %s failed to load from file (file == NULL), loading from filesystem\n"), filename->c_str());
			goto LoadFileSystem;
		}

		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		if (size)
		{
			buffer = (unsigned char *)malloc(size);
			if (buffer)
				fread(buffer, 1, size, file);
			else
				printf(AY_OBFUSCATE("LoadJson: %s failed to load from file (malloc failed), loading from filesystem\n"), filename->c_str());
		}
		else
			printf(AY_OBFUSCATE("LoadJson: %s failed to load from file (size <= 0), loading from filesystem\n"), filename->c_str());

		fclose(file);

		if (buffer)
			goto SetBuffer;
	}

LoadFileSystem:
	FileHandle_t fh = g_pFileSystem->Open(filename->c_str(), "r", 0);
	if (!fh)
	{
		printf(AY_OBFUSCATE("LoadJson: %s failed to load from filesystem (fh == NULL), loading hardcoded values\n"), filename->c_str());
		goto LoadDefaultBuf;
	}

	size = g_pFileSystem->Size(fh);
	if (size)
	{
		buffer = (unsigned char *)malloc(size);
		if (buffer)
			g_pFileSystem->Read(buffer, size, fh);
		else
			printf(AY_OBFUSCATE("LoadJson: %s failed to load from filesystem (malloc failed), loading hardcoded values\n"), filename->c_str());
	}
	else
		printf(AY_OBFUSCATE("LoadJson: %s failed to load from filesystem (size <= 0), loading hardcoded values\n"), filename->c_str());

	g_pFileSystem->Close(fh);

	if (buffer)
		goto SetBuffer;

LoadDefaultBuf:
	buffer = defaultBuf;
	size = defaultBufSize;

SetBuffer:
	*oriBuf = std::string((char *)buffer, (char *)buffer + size);

	return 1;
}

CreateHook(__stdcall, int, LoadJson, std::string *filename, std::string *buffer)
{
	if (dediCsv.find(*filename) != dediCsv.end())
	{
		switch (dediCsv[*filename])
		{
		case ZombieSkillProperty_Crazy:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Crazy, sizeof(g_ZombieSkillProperty_Crazy));
		case ZombieSkillProperty_JumpBuff:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_JumpBuff, sizeof(g_ZombieSkillProperty_JumpBuff));
		case ZombieSkillProperty_ArmorUp:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_ArmorUp, sizeof(g_ZombieSkillProperty_ArmorUp));
		case ZombieSkillProperty_Heal:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Heal, sizeof(g_ZombieSkillProperty_Heal));
		case ZombieSkillProperty_ShieldBuf:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_ShieldBuf, sizeof(g_ZombieSkillProperty_ShieldBuf));
		case ZombieSkillProperty_Cloacking:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Cloacking, sizeof(g_ZombieSkillProperty_Cloacking));
		case ZombieSkillProperty_Trap:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Trap, sizeof(g_ZombieSkillProperty_Trap));
		case ZombieSkillProperty_Smoke:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Smoke, sizeof(g_ZombieSkillProperty_Smoke));
		case ZombieSkillProperty_VoodooHeal:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_VoodooHeal, sizeof(g_ZombieSkillProperty_VoodooHeal));
		case ZombieSkillProperty_Shock:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Shock, sizeof(g_ZombieSkillProperty_Shock));
		case ZombieSkillProperty_Rush:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Rush, sizeof(g_ZombieSkillProperty_Rush));
		case ZombieSkillProperty_Pile:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Pile, sizeof(g_ZombieSkillProperty_Pile));
		case ZombieSkillProperty_Bat:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Bat, sizeof(g_ZombieSkillProperty_Bat));
		case ZombieSkillProperty_Stiffen:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Stiffen, sizeof(g_ZombieSkillProperty_Stiffen));
		case ZombieSkillProperty_SelfDestruct:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_SelfDestruct, sizeof(g_ZombieSkillProperty_SelfDestruct));
		case ZombieSkillProperty_Penetration:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Penetration, sizeof(g_ZombieSkillProperty_Penetration));
		case ZombieSkillProperty_Revival:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Revival, sizeof(g_ZombieSkillProperty_Revival));
		case ZombieSkillProperty_Telleport:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Telleport, sizeof(g_ZombieSkillProperty_Telleport));
		case ZombieSkillProperty_Boost:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Boost, sizeof(g_ZombieSkillProperty_Boost));
		case ZombieSkillProperty_BombCreate:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_BombCreate, sizeof(g_ZombieSkillProperty_BombCreate));
		case ZombieSkillProperty_Flying:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Flying, sizeof(g_ZombieSkillProperty_Flying));
		case ZombieSkillProperty_Fireball:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Fireball, sizeof(g_ZombieSkillProperty_Fireball));
		case ZombieSkillProperty_DogShoot:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_DogShoot, sizeof(g_ZombieSkillProperty_DogShoot));
		case ZombieSkillProperty_ViolentRush:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_ViolentRush, sizeof(g_ZombieSkillProperty_ViolentRush));
		case ZombieSkillProperty_WebShooter:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_WebShooter, sizeof(g_ZombieSkillProperty_WebShooter));
		case ZombieSkillProperty_WebBomb:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_WebBomb, sizeof(g_ZombieSkillProperty_WebBomb));
		case ZombieSkillProperty_Protect:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Protect, sizeof(g_ZombieSkillProperty_Protect));
		case ZombieSkillProperty_ChargeSlash:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_ChargeSlash, sizeof(g_ZombieSkillProperty_ChargeSlash));
		case ZombieSkillProperty_Claw:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Claw, sizeof(g_ZombieSkillProperty_Claw));
		case ZombieSkillProperty_Confuse:
			return LoadJson(filename, buffer, g_ZombieSkillProperty_Confuse, sizeof(g_ZombieSkillProperty_Confuse));
		}
	}

	return g_pfnLoadJson(filename, buffer);
}

enum metaDataType
{
	zipMetadata,
	binToJsonMetadata,
	binMetadata
};

metaDataType GetMetadataType(int metaDataID)
{
	switch (metaDataID)
	{
	case 0:
	case 1:
	case 2:
	case 9:
	case 17:
	case 18:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 44:
	case 45:
	case 46:
	case 48:
	case 50:
	case 51:
	case 52:
	case 53:
	case 56:
	case 58:
	case 59:
	case 60:
	case 61:
	case 62:
	case 63:
		return zipMetadata;
	case 6:
	case 15:
	case 16:
	case 65:
		return binToJsonMetadata;
	default:
		return binMetadata;
	}
}

const char *GetMetadataName(int metaDataID)
{
	switch (metaDataID)
	{
	case 0:
		return "MapList.csv";
	case 1:
		return "ClientTable.csv";
	case 2:
		return "ModeList.csv";
	case 3:
		return "WeaponAssem";
	case 4:
		return "ItemBox";
	case 6:
		return "WeaponPaints";
	case 8:
		return "WeaponAuction";
	case 9:
		return "MatchOption.csv";
	case 15:
		return "ZombieWarWeaponList";
	case 16:
		return "RandomWeaponList";
	case 17:
		return "weaponparts.csv";
	case 18:
		return "MileageShop.csv";
	case 20:
		return "SeasonPass";
	case 24:
		return "GameModeList.csv";
	case 25:
		return "badwordadd.csv";
	case 26:
		return "badworddel.csv";
	case 27:
		return "progress_unlock.csv";
	case 28:
		return "ReinforceMaxLv.csv";
	case 29:
		return "ReinforceMaxExp.csv";
	case 30:
		return "ReinforceItemsExp";
	case 31:
		return "Expedition";
	case 32:
		return "Item.csv";
	case 33:
		return "voxel_list.csv";
	case 34:
		return "voxel_item.csv";
	case 35:
		return "CodisData.csv";
	case 36:
		return "HonorMoneyShop.csv";
	case 37:
		return "ItemExpireTime.csv";
	case 38:
		return "scenariotx_common.json";
	case 39:
		return "scenariotx_dedi.json";
	case 40:
		return "shopitemlist_dedi.json";
	case 41:
		return "EpicPieceShop.csv";
	case 42:
		return "WeaponProp.json";
	case 43:
		return "InventorySlot";
	case 44:
		return "BlackBadgeShop.csv";
	case 45:
		return "ppsystem.json";
	case 46:
		return "classmastery.json";
	case 48:
		return "ZBCompetitive.json";
	case 49:
		return "WeaponEventAuction";
	case 50:
		return "ModeEvent.csv";
	case 51:
		return "EventShop.csv";
	case 52:
		return "FamilyTotalWarMap.csv";
	case 53:
		return "FamilyTotalWar.json";
	case 56:
		return "WeaponAscend.csv";
	case 57:
		return "WeaponAscendExp";
	case 58:
		return "PerkParam.csv";
	case 59:
		return "AnniversaryShop.csv";
	case 60:
		return "GlobalAnniversaryLottery.json";
	case 61:
		return "AnniversaryLottery.json";
	case 62:
		return "AnniversaryTicket.json";
	case 63:
		return "Synthesis.csv";
	case 65:
		return "VoxelConfigList";
	}
	return NULL;
}

#pragma region Packet
std::string readStr(char *buffer, int offset)
{
	std::string result;

	char curChar = buffer[offset]; offset++;
	while (curChar != '\0')
	{
		result += curChar;
		curChar = buffer[offset]; offset++;
	}

	return result;
}

void *g_pPacketMetadataParse;

CreateHookClass(int, Packet_Metadata_Parse, void *packetBuffer, int packetSize)
{
	g_pPacketMetadataParse = ptr;

	if (g_bIgnoreMetadata)
	{
		return false;
	}

	unsigned char metaDataID = *(unsigned char *)packetBuffer;
	printf("Received metadata ID %d\n", metaDataID);

	metaDataType metaDataType = GetMetadataType(metaDataID);

	const char *metaDataName = GetMetadataName(metaDataID);

	if (g_bDumpMetadata)
	{
		char name[MAX_PATH];
		FILE *file = NULL;

		CreateDirectory("MetadataDump", NULL);

		switch (metaDataType)
		{
		case zipMetadata:
		{
			sprintf_s(name, "MetadataDump/Metadata_%s.zip", metaDataName);
			break;
		}
		case binToJsonMetadata:
		{
			sprintf_s(name, "MetadataDump/%s.json", metaDataName);
			file = fopen(name, "wb");
			if (!file)
			{
				printf("Can't open '%s' file to write metadata dump\n", name);
			}
			else
			{
				switch (metaDataID)
				{
				case 6:
				{
					fwrite("{\n\t\"Version\": 1,\n", 17, 1, file);

					int offset = 1;
					int size = *((unsigned short *)((char *)packetBuffer + offset));
					offset += 2;

					for (int i = 0; i < size; i++)
					{
						int weaponID = *((unsigned short *)((char *)packetBuffer + offset));
						offset += 2;
						int size2 = *((unsigned short *)((char *)packetBuffer + offset));
						offset += 2;

						char weaponIDStr[32];
						int weaponIDSize = sprintf_s(weaponIDStr, "\t\"%d\": {\n\t\t\"Paints\": [\n", weaponID);
						fwrite(weaponIDStr, weaponIDSize, 1, file);

						for (int j = 0; j < size2; j++)
						{
							int paintID = *((unsigned short *)((char *)packetBuffer + offset));
							offset += 2;

							char paintIDStr[16];
							int paintIDSize = sprintf_s(paintIDStr, "\t\t\t%d", paintID);
							fwrite(paintIDStr, paintIDSize, 1, file);

							if (size2 - 1 != j)
								fwrite(",", 1, 1, file);

							fwrite("\n", 1, 1, file);
						}

						fwrite("\t\t]\n\t}", 6, 1, file);

						if (size - 1 != i)
							fwrite(",", 1, 1, file);

						fwrite("\n", 1, 1, file);
					}

					fwrite("}", 1, 1, file);
					break;
				}
				case 15:
				{
					fwrite("{\n\t\"Version\": 1,\n\t\"Weapons\": [\n", 31, 1, file);

					int offset = 1;
					int size = *((unsigned short *)((char *)packetBuffer + offset));
					offset += 2;

					for (int i = 0; i < size; i++)
					{
						int itemID = *((unsigned long *)((char *)packetBuffer + offset));
						offset += 4;

						char itemIDStr[16];
						int itemIDSize = sprintf_s(itemIDStr, "\t\t%d", itemID);
						fwrite(itemIDStr, itemIDSize, 1, file);

						if (size - 1 != i)
							fwrite(",", 1, 1, file);

						fwrite("\n", 1, 1, file);
					}

					fwrite("\t]\n}", 4, 1, file);
					break;
				}
				case 16:
				{
					fwrite("{\n\t\"Version\": 1,\n", 17, 1, file);

					int offset = 1;
					int size = *((unsigned long *)((char *)packetBuffer + offset));
					offset += 4;

					for (int i = 0; i < size; i++)
					{
						int itemID = *((unsigned long *)((char *)packetBuffer + offset));
						offset += 4;
						int size2 = *((unsigned long *)((char *)packetBuffer + offset));
						offset += 4;

						char itemIDStr[16];
						int itemIDSize = sprintf_s(itemIDStr, "\t\"%d\": {\n", itemID);
						fwrite(itemIDStr, itemIDSize, 1, file);

						for (int j = 0; j < size2; j++)
						{
							int modeFlag = *((unsigned char *)((char *)packetBuffer + offset));
							offset++;
							int dropRate = *((unsigned long *)((char *)packetBuffer + offset));
							offset += 4;
							int enhanceProbability = *((unsigned long *)((char *)packetBuffer + offset));
							offset += 4;

							char modeFlagStr[16];
							int modeFlagSize = sprintf_s(modeFlagStr, "\t\t\"%d\": {\n", modeFlag);
							fwrite(modeFlagStr, modeFlagSize, 1, file);

							char dropRateStr[32];
							int dropRateSize = sprintf_s(dropRateStr, "\t\t\t\"DropRate\": %d,\n", dropRate);
							fwrite(dropRateStr, dropRateSize, 1, file);

							char enhanceProbabilityStr[32];
							int enhanceProbabilitySize = sprintf_s(enhanceProbabilityStr, "\t\t\t\"EnhanceProbability\": %d\n", enhanceProbability);
							fwrite(enhanceProbabilityStr, enhanceProbabilitySize, 1, file);

							fwrite("\t\t}", 3, 1, file);

							if (size2 - 1 != j)
								fwrite(",", 1, 1, file);

							fwrite("\n", 1, 1, file);
						}

						fwrite("\t}", 2, 1, file);

						if (size - 1 != i)
							fwrite(",", 1, 1, file);

						fwrite("\n", 1, 1, file);
					}

					fwrite("}", 1, 1, file);
					break;
				}
				case 65:
				{
					fwrite("{\n\t\"Version\": 1,\n\t\"VoxelConfigList\": [\n", 39, 1, file);

					int offset = 1;
					int size = *((unsigned char*)((char*)packetBuffer + offset)); offset++;

					for (int i = 0; i < size; i++)
					{
						int unk = *((unsigned char*)((char*)packetBuffer + offset)); offset++;
						std::string vxlURL = readStr((char*)packetBuffer, offset); offset += vxlURL.size() + 1;
						std::string vmgURL = readStr((char*)packetBuffer, offset); offset += vmgURL.size() + 1;

						char unkStr[19];
						int unkStrSize = sprintf_s(unkStr, "\t\t{\n\t\t\t\"Unk\": %d,", unk);
						fwrite(unkStr, unkStrSize, 1, file);

						fwrite("\n\t\t\t\"VxlURL\": \"", 15, 1, file);
						fwrite(vxlURL.c_str(), vxlURL.size(), 1, file);
						fwrite("\",\n\t\t\t\"VmgURL\": \"", 17, 1, file);
						fwrite(vmgURL.c_str(), vmgURL.size(), 1, file);
						fwrite("\",\n\t\t\t\"HTTPIPList\": [\n", 22, 1, file);

						int size2 = *((unsigned char*)((char*)packetBuffer + offset)); offset++;

						for (int j = 0; j < size2; j++)
						{
							std::string httpIP = readStr((char*)packetBuffer, offset); offset += httpIP.size() + 1;

							fwrite("\t\t\t\t{\n\t\t\t\t\t\"IP\": \"", 18, 1, file);
							fwrite(httpIP.c_str(), httpIP.size(), 1, file);
							fwrite("\",\n\t\t\t\t\t\"Ports\": [", 18, 1, file);

							int size3 = *((unsigned char*)((char*)packetBuffer + offset)); offset++;

							for (int k = 0; k < size3; k++)
							{
								int port = *((unsigned short*)((char*)packetBuffer + offset)); offset += 2;
								
								char portStr[7];
								int portStrSize = sprintf_s(portStr, " %d", port);
								fwrite(portStr, portStrSize, 1, file);

								if (size3 - 1 != k)
									fwrite(",", 1, 1, file);
							}

							fwrite(" ]\n\t\t\t\t}", 8, 1, file);

							if (size2 - 1 != j)
								fwrite(",", 1, 1, file);

							fwrite("\n", 1, 1, file);
						}

						fwrite("\t\t\t]\n\t\t}", 8, 1, file);

						if (size - 1 != i)
							fwrite(",", 1, 1, file);

						fwrite("\n", 1, 1, file);
					}

					fwrite("\t]\n}", 4, 1, file);
					break;
				}
				}
				fclose(file);
			}
			break;
		}
		case binMetadata:
		{
			if (metaDataName)
				sprintf_s(name, "MetadataDump/Metadata_%s.bin", metaDataName);
			else
				sprintf_s(name, "MetadataDump/Metadata_Unk%d.bin", metaDataID);
			break;
		}
		}

		if (metaDataType != binToJsonMetadata)
		{
			if (metaDataType == zipMetadata && (*((unsigned char *)((char *)packetBuffer + 1)) != 0x5 && *((unsigned char *)((char *)packetBuffer + 1)) != 0x1)) // 0x5 - Full zip, 0x1 - First part, 0x2 - Second part, 0x3 - Third part, 0x4 - Last part
			{
				file = fopen(name, "ab");
			}
			else
			{
				file = fopen(name, "wb");
			}

			if (!file)
			{
				printf("Can't open '%s' file to write metadata dump\n", name);
			}
			else
			{
				if (metaDataType == zipMetadata)
				{
					fwrite(((char *)packetBuffer + 4), *((unsigned short *)((char *)packetBuffer + 2)), 1, file);
				}
				else
				{
					fwrite(packetBuffer, packetSize, 1, file);
				}
				fclose(file);
			}
		}
	}

	if (g_bWriteMetadata && metaDataType == zipMetadata)
	{
		HZIP hMetaData = CreateZip(0, MAX_ZIP_SIZE, ZIP_MEMORY);

		if (!hMetaData)
		{
			printf("CreateZip returned NULL.\n");
			return g_pfnPacket_Metadata_Parse(ptr, packetBuffer, packetSize);
		}

		char path[MAX_PATH];
		sprintf(path, "Metadata/%s", metaDataName);
		printf("Writing metadata from %s\n", path);

		if (ZipAdd(hMetaData, metaDataName, path, 0, ZIP_FILENAME))
		{
			printf("ZipAdd returned error.\n");
			return g_pfnPacket_Metadata_Parse(ptr, packetBuffer, packetSize);
		}

		void *buffer = nullptr;
		unsigned long length = 0;
		ZipGetMemory(hMetaData, &buffer, &length);

		if (length == 0)
		{
			printf("ZipGetMemory returned zero length.\n");
			return g_pfnPacket_Metadata_Parse(ptr, packetBuffer, packetSize);
		}

		std::vector<unsigned char> destBuffer;
		std::vector<unsigned char> metaDataBuffer((char *)buffer, (char *)buffer + length);

		destBuffer.push_back(metaDataID);
		destBuffer.push_back(0x05);
		destBuffer.push_back((unsigned char)(length & 0xFF));
		destBuffer.push_back((unsigned char)(length >> 8));
		destBuffer.insert(destBuffer.end(), metaDataBuffer.begin(), metaDataBuffer.end());

		CloseZip(hMetaData);

		return g_pfnPacket_Metadata_Parse(ptr, static_cast<void *>(destBuffer.data()), destBuffer.size());
	}

	return g_pfnPacket_Metadata_Parse(ptr, packetBuffer, packetSize);
}

void Metadata_RequestAll()
{
	std::vector<unsigned char> destBuffer;

	for (int i = 0; i < 66; i++)
	{
		destBuffer.push_back(0xFF);
		destBuffer.push_back(i);

		for (int j = 0; j < 16; j++)
			destBuffer.push_back(0x00);

		g_pfnPacket_Metadata_Parse(g_pPacketMetadataParse, static_cast<void *>(destBuffer.data()), destBuffer.size());
		destBuffer.clear();
	}
}

int __fastcall Hook_Packet_Hack_Parse(void *_this, int a2, void *packetBuffer, int packetSize)
{
	return 1;
}
#pragma endregion

void __fastcall LoginDlg_OnCommand(void *_this, int r, const char *command)
{
	if (!strcmp(command, "Login"))
	{
		DWORD **v3 = (DWORD **)_this;
		char login[256];
		char password[256];

		// #1
		(*(void(__thiscall **)(DWORD *, char *, signed int))(*v3[111] + 656))(v3[111], login, 256);	   // textentry->GetText() // before 23.12.23 *v3[109] + 620 // before 25.03.19 *v3[109] + 628
		(*(void(__thiscall **)(DWORD *, char *, signed int))(*v3[112] + 656))(v3[112], password, 256); // before 25.03.19 *v3[110] + 628

		wchar_t buf[512];
		swprintf(buf, L"/login %S %S", login, password);
		if (g_pChattingManager)
			g_pChattingManager->PrintToChat(1, buf);
		return;
	}
	else if (!strcmp(command, "Register"))
	{
		DWORD **v3 = (DWORD **)_this;
		char login[256];
		char password[256];

		// #2
		(*(void(__thiscall **)(DWORD *, char *, signed int))(*v3[111] + 656))(v3[111], login, 256);	   // textentry->GetText() //before 25.03.19 *v3[109] + 628
		(*(void(__thiscall **)(DWORD *, char *, signed int))(*v3[112] + 656))(v3[112], password, 256); // before 25.03.19 *v3[110] + 628

		wchar_t buf[512];
		swprintf(buf, L"/register %S %S", login, password);
		if (g_pChattingManager)
			g_pChattingManager->PrintToChat(1, buf);
		return;
	}

	g_pfnLoginDlg_OnCommand(_this, command);
}

bool bShowLoginDlg = false;
int __fastcall GameUI_RunFrame(void *_this)
{
	if (!bShowLoginDlg)
	{
		if (strlen(g_pLogin) != 0 || strlen(g_pPassword) != 0)
		{
			Sleep(500);

			wchar_t buf[512];
			swprintf(buf, g_bRegister ? L"/register %S %S" : L"/login %S %S", g_pLogin, g_pPassword);
			if (g_pChattingManager)
				g_pChattingManager->PrintToChat(1, buf);
		}
	}
	return g_pfnGameUI_RunFrame(_this);
}

void CSO_Bot_Add()
{
	// get current botmgr ptr
	DWORD dwBotManagerPtr = FindPattern(BOT_MANAGER_PTR_SIG_CSNZ, BOT_MANAGER_PTR_MASK_CSNZ, g_dwMpBase, g_dwMpBase + g_dwMpSize, 1);
	if (!dwBotManagerPtr)
	{
		return;
	}

	g_pBotManager = **((CCSBotManager ***)(dwBotManagerPtr));
	if (g_pBotManager == NULL)
	{
		return;
	}

	int side = 0;
	int argc = g_pEngine->Cmd_Argc();
	if (argc > 0)
	{
		side = atoi(g_pEngine->Cmd_Argv(1));
	}
	g_pBotManager->Bot_Add(side);
}

const char *__fastcall Hook_GetSSLProtocolName(void *_this)
{
	return "None";
}

CreateHookClassType(void *, SocketConstructor, int, int a2, int a3, char a4)
{
	*(DWORD *)((int)ptr + 72) = (DWORD)g_pfnEVP_CIPHER_CTX_new();
	*(DWORD *)((int)ptr + 76) = (DWORD)g_pfnEVP_CIPHER_CTX_new();
	*(DWORD *)((int)ptr + 80) = (DWORD)g_pfnEVP_CIPHER_CTX_new();
	*(DWORD *)((int)ptr + 84) = (DWORD)g_pfnEVP_CIPHER_CTX_new();

	return g_pfnSocketConstructor(ptr, a2, a3, a4);
}

static int g_iLobbyKickID = 0x0;
static char g_szLobbyKickText[256] = { 0 };

static DWORD** g_pNetworkManager = nullptr;
typedef int(__thiscall *DisconnectClient)(DWORD* pthis, BOOL reconnect, const char* reason);
DisconnectClient g_pfnDisconnectClient = nullptr;

CreateHookClass(int, ReadPacket, char *outBuf, int len, unsigned short *outLen, bool initialMsg)
{
	int result = g_pfnReadPacket(ptr, outBuf, len, outLen, initialMsg);

	if (!initialMsg && result == 0 && g_bDumpAll)
	{
		CreateDirectory("Packets", NULL);

		static int directoryCounter = 0;

		if (!directoryCounter)
		{
			while (true)
			{
				char directory[MAX_PATH];
				snprintf(directory, sizeof(directory), "Packets/%d", ++directoryCounter);

				DWORD dwAttr = GetFileAttributes(directory);
				if (dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
				{
					continue;
				}

				CreateDirectory(directory, NULL);
				break;
			}
		}

		unsigned char *buf = (unsigned char *)(outBuf);
		unsigned short dataLen = *outLen;

		static int packetCounter = 0;

		char filename[MAX_PATH];
		bool moreInfo = true;
		if (moreInfo)
			snprintf(filename, sizeof(filename), "Packets/%d/Packet_%d_ID_%d_%d.bin", directoryCounter, packetCounter++, buf[0], dataLen);
		else
			snprintf(filename, sizeof(filename), "Packets/%d/Packet_%d.bin", directoryCounter, packetCounter++);

		FILE *file = fopen(filename, "wb");
		fwrite(buf, dataLen, 1, file);
		fclose(file);
	}

	return result;
}

template<typename T>
void WriteToVector(std::vector<unsigned char>& vec, const T& value)
{
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&value);
	vec.insert(vec.end(), bytes, bytes + sizeof(T));
}

CreateHookClass(int, WritePacket, byte *Src, unsigned int Size)
{
	return g_pfnWritePacket(ptr, Src, Size);
}

CreateHookClass(void, LogToErrorLog, int logFileId, char *buffer, int size)
{
	printf("[%s.log] %.*s", logFileId == 3 ? "Error" : "nxa", size, buffer);

	if (size <= 0 || buffer[size - 1] != '\n')
		printf("\n");

	g_pfnLogToErrorLog(ptr, logFileId, buffer, size);
}

CreateHook(WINAPI, void, OutputDebugStringAHook, LPCSTR lpOutString)
{
	printf("[OutputDebugString] %s", lpOutString);

	if (!*lpOutString || lpOutString[strlen(lpOutString) - 1] != '\n')
		printf("\n");

	g_pfnOutputDebugStringAHook(lpOutString);
}

CreateHook(__cdecl, int, HolePunch_GetUserSocketInfo, int userID, char *data)
{
	auto ret = g_pfnHolePunch_GetUserSocketInfo(userID, data);

	data[0] = 2; // unsafety method, since other places port are corrected

	short port = (short &)data[14];
	in_addr ip = (in_addr &)data[16];

	printf("[HolePunch_GetUserSocketInfo] ret: %d | UserID: %d, %s:%d\n", ret, userID, inet_ntoa(ip), ntohs(port));

	return ret;
}

DWORD GetLauncherCompilationTime()
{
	char exePath[MAX_PATH]{};
	if (!GetModuleFileNameA(nullptr, exePath, MAX_PATH))
		return 0;

	char* lastSlash = strrchr(exePath, '\\');
	if (!lastSlash)
		return 0;

	*(lastSlash + 1) = '\0';

	strcat_s(exePath, "cstrike-online.exe");

	HANDLE hFile = CreateFileA(
		exePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	HANDLE hMapping = CreateFileMappingA(
		hFile,
		nullptr,
		PAGE_READONLY,
		0,
		0,
		nullptr
	);

	if (!hMapping)
	{
		CloseHandle(hFile);
		return 0;
	}

	LPVOID base = MapViewOfFile(
		hMapping,
		FILE_MAP_READ,
		0,
		0,
		0
	);

	if (!base)
	{
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return 0;
	}

	DWORD timestamp = 0;

	auto dos = (PIMAGE_DOS_HEADER)base;
	if (dos->e_magic == IMAGE_DOS_SIGNATURE)
	{
		auto nt = (PIMAGE_NT_HEADERS)((BYTE*)base + dos->e_lfanew);
		if (nt->Signature == IMAGE_NT_SIGNATURE)
		{
			timestamp = nt->FileHeader.TimeDateStamp;
		}
	}

	UnmapViewOfFile(base);
	CloseHandle(hMapping);
	CloseHandle(hFile);
	return timestamp;
}

CreateHook(__cdecl, DWORD, GetPECompilationTime, const char* moduleName)
{
	if (strcmp(moduleName, "cstrike-online.exe") == 0)
		return GetLauncherCompilationTime();
	else
		return g_pfnGetPECompilationTime(moduleName);
}

CreateHookClass(int, CMapEntityManager)
{
	*g_pFreezeTime = 20;
	return g_pfnCMapEntityManager(ptr);
}

// hw.dll: sub_2541E90 returns the 48-byte weapon info record for a weapon-table index (dword_3F23FC8 = vector begin,
// the dword after it = vector end). Its caller gets the index from a hash lookup that returns -1 for unknown items and
// nothing checks it, so an item the client has no weapon info for (or a table that is not built yet) reads outside the
// table and crashes on map load. Unknown indices get an all-zero record instead.
// hw.dll: sub_2542650 maps an item id to its weapon-table index and returns -1 for items the client has no weapon info for.
// Only logs which item ids hit that case (once per id), the result is passed through unchanged.
CreateHookClass(int, ItemToWeaponIndex, int itemId)
{
	return 0;
}

// hw.dll: IGBuyMenuMgr refresh (sub_25471C0). It only rebuilds the weapon restriction flags (vtable +0x80, which
// reads the current game mode) when the requested tab key differs from the cached one (this+0x1A63C) or when forced, and
// it remembers the mode it built for in this[2]. A new match in another mode that opens the same tab therefore keeps the
// restrictions of the previous mode (e.g. the Zombie Hero Classic weapon list). When the current game mode differs from
// the one the list was built for, the cached key is invalidated so the lists get rebuilt.
static DWORD g_dwGameModeObject = 0;      // address of the engine's game mode object (sub_25FCFA0 returns it)
static DWORD g_dwBuyMenuKeyOffset = 0;    // offset of the cached tab key inside IGBuyMenuMgr

CreateHookClass(int, BuyMenuRefresh, int key, int force)
{
	return 0;
}

static DWORD *g_pWeaponInfoTable = NULL; // &dword_3F23FC8

CreateHookClass(char *, WeaponInfoLookup, int index)
{
	return nullptr;
}

void CreateDebugConsole()
{
	AllocConsole();

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	SetConsoleTitleA("CSO launcher debug console");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	setlocale(LC_ALL, "");
}

// bool __thiscall CSOWebmDecoder_Load(void *this, void *a2, char a3, int a4)
CreateHookClass(bool, CSOWebmDecoder_Load, void *a2, char a3, int a4)
{
	const char *a2_file = (const char *)((DWORD *)a2)[0];

	if (!strcmp(a2_file, "resource/bg_bink.webm"))
	{
		static std::vector<const char *> randomWebmFiles = {
			"resource/bg_bink.webm",
			"resource/bg_bink_1.webm",
			"resource/bg_bink_2.webm",
		};

		int randomIndex = rand() % randomWebmFiles.size();
		const char *randomWebmFile = randomWebmFiles[randomIndex];

		((DWORD *)a2)[0] = (DWORD)randomWebmFile;
		bool result = g_pfnCSOWebmDecoder_Load(ptr, a2, a3, a4);
		if (result)
			return result;
		((DWORD *)a2)[0] = (DWORD)a2_file;
	}
	return g_pfnCSOWebmDecoder_Load(ptr, a2, a3, a4);
}

CreateHookClass(bool, CSONWrapper_Login, int a2, int a3, int a4)
{
	return true;
}

#define QWORD unsigned long long

template<typename Ret, typename... Args>
inline Ret CallVFunc(void* instance, int offset, Args... args) {
    typedef Ret (__thiscall* VFn)(void*, Args...);
    return (*(VFn*)(*(DWORD*)instance + offset))(instance, args...);
}

typedef int (__thiscall *IPanel_GetPosition)(DWORD *pthis, int *a2, int *a3);
typedef int (__thiscall *IPanel_SetPosition)(DWORD *pthis, int a2, int a3);

IPanel_GetPosition g_pfnIPanel_GetPosition = nullptr;
IPanel_SetPosition g_pfnIPanel_SetPosition = nullptr;

CreateHookClass(int, CItemManagerDlgVer3_SetVisible, bool IsShop)
{
	if (!g_pfnIPanel_GetPosition)
	{
		g_pfnIPanel_GetPosition = (IPanel_GetPosition)(g_dwGameUIBase + 0x6f44a0);
	}

	if (!g_pfnIPanel_SetPosition)
	{
		g_pfnIPanel_SetPosition = (IPanel_SetPosition)(g_dwGameUIBase + 0x6f88a0);
	}

	int v11 = 0;
	int v10 = 0;

	DWORD* pthis = (DWORD*)ptr;
	DWORD* mainpanel = (DWORD*)pthis[97];

	if (IsShop) {
		g_pfnIPanel_GetPosition((DWORD*)mainpanel, &v11, &v10);

		int v8 = 973 - v11;
		g_pfnIPanel_SetPosition((DWORD*)pthis[102], v8, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[104], v8 + 5, 23);
		g_pfnIPanel_SetPosition((DWORD*)pthis[103], v8 + 47, 20);

		int v82 = -4 - v11 + v8;
		g_pfnIPanel_SetPosition((DWORD*)pthis[97], v82, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[98], v82 + 5, 23);
		g_pfnIPanel_SetPosition((DWORD*)pthis[99], v82 + 70, 20);
		g_pfnIPanel_SetPosition((DWORD*)pthis[101], v82 + 160, 18);

		int x3, y3;
		g_pfnIPanel_GetPosition((DWORD*)pthis[94], &x3, &y3);

		int v9 = -4 - x3 + v82;
		g_pfnIPanel_SetPosition((DWORD*)pthis[94], v9, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[95], v9 + 5, 23);
		g_pfnIPanel_SetPosition((DWORD*)pthis[96], v9 + 47, 20);

		int v10 = 38 - v11 + v9;
		g_pfnIPanel_SetPosition((DWORD*)pthis[92], v10, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[91], v10 + 3, 23);

		CallVFunc<int>((DWORD*)pthis[97], 172, 1);
		CallVFunc<int>((DWORD*)pthis[98], 172, 1);
		CallVFunc<int>((DWORD*)pthis[99], 172, 1);
		CallVFunc<int>((DWORD*)pthis[101], 172, 1);
		CallVFunc<int>((DWORD*)pthis[94], 172, 1);
		CallVFunc<int>((DWORD*)pthis[95], 172, 1);
		CallVFunc<int>((DWORD*)pthis[96], 172, 1);
		CallVFunc<int>((DWORD*)pthis[92], 172, 1);
		CallVFunc<int>((DWORD*)pthis[91], 172, 1);
		CallVFunc<int>((DWORD*)pthis[93], 172, 0);
		CallVFunc<int>((DWORD*)pthis[105], 172, 1);
		CallVFunc<int>((DWORD*)pthis[102], 172, 1);
		return CallVFunc<int>((DWORD*)pthis[104], 172, 1);
	} else {
		g_pfnIPanel_GetPosition((DWORD*)mainpanel, &v11, &v10);

		int v8 = 1060 - v11;
		g_pfnIPanel_SetPosition((DWORD*)pthis[102], v8, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[104], v8 + 5, 23);
		g_pfnIPanel_SetPosition((DWORD*)pthis[103], v8 + 47, 20);

		int v82 = -4 - v11 + v8;
		g_pfnIPanel_SetPosition((DWORD*)pthis[97], v82, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[98], v82 + 5, 23);
		g_pfnIPanel_SetPosition((DWORD*)pthis[99], v82 + 70, 20);
		g_pfnIPanel_SetPosition((DWORD*)pthis[101], v82 + 160, 18);

		int x3, y3;
		g_pfnIPanel_GetPosition((DWORD*)pthis[94], &x3, &y3);

		int v9 = -4 - x3 + v82;
		g_pfnIPanel_SetPosition((DWORD*)pthis[94], v9, 19);
		g_pfnIPanel_SetPosition((DWORD*)pthis[95], v9 + 5, 23);
		g_pfnIPanel_SetPosition((DWORD*)pthis[96], v9 + 47, 20);

		CallVFunc<int>((DWORD*)pthis[102], 172, 1);
		CallVFunc<int>((DWORD*)pthis[104], 172, 1);
		CallVFunc<int>((DWORD*)pthis[103], 172, 1);
		CallVFunc<int>((DWORD*)pthis[97], 172, 1);
		CallVFunc<int>((DWORD*)pthis[98], 172, 1);
		CallVFunc<int>((DWORD*)pthis[99], 172, 1);
		CallVFunc<int>((DWORD*)pthis[101], 172, 1);
		CallVFunc<int>((DWORD*)pthis[94], 172, 1);
		CallVFunc<int>((DWORD*)pthis[95], 172, 1);
		CallVFunc<int>((DWORD*)pthis[96], 172, 1);
		CallVFunc<int>((DWORD*)pthis[105], 172, 0);
		CallVFunc<int>((DWORD*)pthis[92], 172, 0);
		CallVFunc<int>((DWORD*)pthis[91], 172, 0);
		CallVFunc<int>((DWORD*)pthis[93], 172, 0);
		CallVFunc<int>((DWORD*)pthis[105], 172, 0);
		CallVFunc<int>((DWORD*)pthis[102], 172, 1);
		CallVFunc<int>((DWORD*)pthis[104], 172, 1);
		return CallVFunc<int>((DWORD*)pthis[103], 172, 1);
	}
}

CreateHookClass(void, VGUI_MessageBox_New, const char* title, const char* text, int type)
{
	if (g_iLobbyKickID) {
		return g_pfnVGUI_MessageBox_New(ptr, title, g_szLobbyKickText, type);
	}

	return g_pfnVGUI_MessageBox_New(ptr, title, text, type);
}

// client.dll: CHudHealth::VidInit health-icon sprite name selection.
// "cmovnz eax, edx" overrides the classic "cross" sprite name with "cross_new" for every
// gamemode except the {11,18,21,25,38,39} group. Generated via IDA Fusion-style signature
// scan (wildcard_operands); the "cross"/"cross_new" string-address immediates are left
// literal since they're static rdata addresses within this same client.dll build.
#define CLIENT_HEALTH_ICON_CLASSIC_SIG_CSO "\xFF\x15\x00\x00\x00\x00\x8B\xC8\x8B\x10\xFF\x92\x00\x00\x00\x00\x83\xC0\x00\x83\xF8\x00\x77\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\x32\xC9\xEB\x00\xB1"
#define CLIENT_HEALTH_ICON_CLASSIC_MASK_CSO "xx????xxxxxx????xx?xx?x?xxx????xxx????xxx?x"

// client.dll: CHudBattery::VidInit armor/suit icon sprite name selection (sibling of the
// health-icon check above, same {11,18,21,25,38,39} mode gate). Four "cmovnz" overrides,
// one each for suit_empty, suit_full, suithelmet_empty, suithelmet_full.
#define CLIENT_BATTERY_ICON_CLASSIC_SIG_CSO "\xFF\x15\x00\x00\x00\x00\x8B\xC8\x8B\x10\xFF\x92\x88\x01\x00\x00\x83\xC0\xF5\x83\xF8\x1C\x77\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\xBF\x00\x00\x00\x00\xC7\x45\x00\x00\x00\x00\x00\xBB\x00\x00\x00\x00\xB8\x00\x00\x00\x00\xEB\x00\xBF\x00\x00\x00\x00\xC7\x45\x00\x00\x00\x00\x00\xBB\x00\x00\x00\x00\xB8"
#define CLIENT_BATTERY_ICON_CLASSIC_MASK_CSO "xx????xxxxxxxxxxxxxxxxx?xxx????xxx????x????xx?????x????x????x?x????xx?????x????x"

// client.dll: CHudAccountBalance::VidInit "$" pickup icons (dollar/minus/plus), same
// {11,18,21,25,38,39} mode gate, three "cmovnz" overrides.
#define CLIENT_ACCOUNTBALANCE_ICON_CLASSIC_SIG_CSO "\xFF\x15\x00\x00\x00\x00\x8B\xC8\x8B\x10\xFF\x92\x88\x01\x00\x00\x83\xC0\xF5\x83\xF8\x1C\x77\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\xBE\x00\x00\x00\x00\xBF\x00\x00\x00\x00\xB8\x00\x00\x00\x00\xEB\x00\xBE\x00\x00\x00\x00\xBF"
#define CLIENT_ACCOUNTBALANCE_ICON_CLASSIC_MASK_CSO "xx????xxxxxxxxxxxxxxxxx?xxx????xxx????x????x????x????x?x????x"

// client.dll: weapon-selection background sprite (bucket UI). "selection" is set
// unconditionally first; the switch's default case (every mode except {11,18,21,25,38,39})
// then overwrites it with "weapon_selection_new". NOPing that overwrite leaves the classic
// "selection" value in place for every gamemode.
#define CLIENT_WEAPONSELECTION_ICON_CLASSIC_SIG_CSO "\xE8\x00\x00\x00\x00\x89\x43\x00\xFF\x15"
#define CLIENT_WEAPONSELECTION_ICON_CLASSIC_MASK_CSO "x????xx?xx"

// client.dll: hostage icon (CHudZombieShelter-family). "hostage" set unconditionally first;
// default case (every mode except {11,18,21,25,38,39}) overwrites it with "hostage_new".
#define CLIENT_HOSTAGE_ICON_CLASSIC_SIG_CSO "\xFF\x15\x00\x00\x00\x00\x8B\xC8\x8B\x10\x8B\x92\x00\x00\x00\x00\xFF\xD2\x83\xC0\xF5\x83\xF8\x1C\x77\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\x68\x70\xF3\xE6\x02\xB9\xD0\x86\x68\x03\xE8\x00\x00\x00\x00\x8B\xF8"
#define CLIENT_HOSTAGE_ICON_CLASSIC_MASK_CSO "xx????xxxxxx????xxxxxxxxx?xxx????xxx????xxxxxxxxxxx????xx"

// client.dll only exists in the game client process, not the dedicated server; this is
// wired up via ClientHookThread (waits for client.dll to load, same as HookThread does for mp.dll)
// so it's ready to go once this hook.cpp is reused in the launcher/client build.
void CSO_PatchClientHealthIconClassic()
{
	DWORD find = FindPattern(CLIENT_HEALTH_ICON_CLASSIC_SIG_CSO, CLIENT_HEALTH_ICON_CLASSIC_MASK_CSO, g_dwClientBase, g_dwClientBase + g_dwClientSize, NULL);
	if (!find)
	{
		MessageBox(NULL, "ClientHealthIconClassic == NULL!!!", "Error", MB_OK);
		return;
	}

	// cmovnz eax, edx (0F 45 C2) -> nop x3: eax always keeps the "cross" (classic) sprite name
	// instead of being overridden to "cross_new" outside the {11,18,21,25,38,39} gamemode group.
	BYTE patch[] = { 0x90, 0x90, 0x90 };
	WriteMemory((void*)(find + 63), patch, sizeof(patch));
}

// client.dll: same gamemode gate as CSO_PatchClientHealthIconClassic, but for CHudBattery's
// four armor/suit icons (suit_empty, suit_full, suithelmet_empty, suithelmet_full).
void CSO_PatchClientBatteryIconClassic()
{
	DWORD find = FindPattern(CLIENT_BATTERY_ICON_CLASSIC_SIG_CSO, CLIENT_BATTERY_ICON_CLASSIC_MASK_CSO, g_dwClientBase, g_dwClientBase + g_dwClientSize, NULL);
	if (!find)
	{
		MessageBox(NULL, "ClientBatteryIconClassic == NULL!!!", "Error", MB_OK);
		return;
	}

	// The compiler turned the four cmovnz into a switch with two branches, so instead of nopping them the
	// game mode is zeroed: "add eax, 0FFFFFFF5h" (83 C0 F5) -> "xor eax, eax ; nop" (31 C0 90) makes the switch
	// always take jump-table index 0, the {11,18,21,25,38,39} case that loads the classic sprite names
	// (suit_empty/suit_full/suithelmet_empty/suithelmet_full) in every gamemode.
	BYTE patch[] = { 0x31, 0xC0, 0x90 };
	WriteMemory((void*)(find + 16), patch, sizeof(patch));
}

// client.dll: same gamemode gate again, for CHudAccountBalance's dollar/minus/plus icons.
void CSO_PatchClientAccountBalanceIconClassic()
{
	DWORD find = FindPattern(CLIENT_ACCOUNTBALANCE_ICON_CLASSIC_SIG_CSO, CLIENT_ACCOUNTBALANCE_ICON_CLASSIC_MASK_CSO, g_dwClientBase, g_dwClientBase + g_dwClientSize, NULL);
	if (!find)
	{
		MessageBox(NULL, "ClientAccountBalanceIconClassic == NULL!!!", "Error", MB_OK);
		return;
	}

	// Same trick as the battery patch: "add eax, 0FFFFFFF5h" -> "xor eax, eax ; nop" so the switch always takes
	// the classic case and the sprite names stay (dollar/minus/plus) in every gamemode.
	BYTE patch[] = { 0x31, 0xC0, 0x90 };
	WriteMemory((void*)(find + 16), patch, sizeof(patch));
}

// client.dll: NOPs the default-case call+store that overwrites the already-classic
// "selection" sprite index with "weapon_selection_new" outside {11,18,21,25,38,39}.
void CSO_PatchClientWeaponSelectionIconClassic()
{
	DWORD find = FindPattern(CLIENT_WEAPONSELECTION_ICON_CLASSIC_SIG_CSO, CLIENT_WEAPONSELECTION_ICON_CLASSIC_MASK_CSO, g_dwClientBase, g_dwClientBase + g_dwClientSize, NULL);
	if (!find)
	{
		MessageBox(NULL, "ClientWeaponSelectionIconClassic == NULL!!!", "Error", MB_OK);
		return;
	}

	// push offset "weapon_selection_new" ; mov ecx, dword_36886D0 ; call sub_27C43E0 ; mov [ebx+24h], eax
	// (18 bytes) -> nop x18, so execution falls straight through to the shared code that
	// still holds the classic "selection" sprite index set earlier in the function.
	BYTE patch[18];
	memset(patch, 0x90, sizeof(patch));
	WriteMemory((void*)(find + 46), patch, sizeof(patch));
}

// client.dll: sub_2812A40 (HUD background panels VidInit) returns early without loading
// hud_weapon_bg_*/hud_character_bg_new_* for {11,18,21,25,38,39}, so those modes have no
// dark panel behind health/armor/ammo. Force the switch's default case (loads them).
#define CLIENT_HUDBG_ALWAYS_SIG_CSO "\xC7\x87\xB8\x02\x00\x00\x00\x00\x80\xBF\xFF\x15\x00\x00\x00\x00\x8B\xC8\x8B\x10\xFF\x92\x88\x01\x00\x00\x83\xC0\xF5\x83\xF8\x1C\x77\x0E"
#define CLIENT_HUDBG_ALWAYS_MASK_CSO "xxxxxxxxxxxx????xxxxxxxxxxxxxxxxxx"

void CSO_PatchClientHudBackgroundAlways()
{
	DWORD find = FindPattern(CLIENT_HUDBG_ALWAYS_SIG_CSO, CLIENT_HUDBG_ALWAYS_MASK_CSO, g_dwClientBase, g_dwClientBase + g_dwClientSize, NULL);
	if (!find)
	{
		MessageBox(NULL, "ClientHudBackgroundAlways == NULL!!!", "Error", MB_OK);
		return;
	}

	// ja short default (77 0E) -> jmp short default (EB 0E)
	BYTE patch[] = { 0xEB };
	WriteMemory((void*)(find + 32), patch, sizeof(patch));
}

void CSO_PatchClientHostageIconClassic()
{

}

struct ModuleInfo
{
	std::string name;
	uintptr_t base;
	uintptr_t size;
};

static std::vector<ModuleInfo> GetLoadedModules()
{
	std::vector<ModuleInfo> modules;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return modules;
	}

	MODULEENTRY32 me = {};
	me.dwSize = sizeof(me);

	if (Module32First(hSnapshot, &me))
	{
		do
		{
			ModuleInfo info;
			info.name = me.szModule;
			info.base = reinterpret_cast<uintptr_t>(me.modBaseAddr);
			info.size = me.modBaseSize;
			modules.push_back(info);
		} while (Module32Next(hSnapshot, &me));
	}

	CloseHandle(hSnapshot);
	return modules;
}

void PrintStackTrace()
{
	void* frames[32] = {};
	USHORT count = RtlCaptureStackBackTrace(1, _countof(frames), frames, NULL);

	std::vector<ModuleInfo> modules = GetLoadedModules();

	printf("[PrintStackTrace] %u frame(s):\n", count);
	for (USHORT i = 0; i < count; i++)
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(frames[i]);

		const ModuleInfo* owner = nullptr;
		for (const ModuleInfo& mod : modules)
		{
			if (address >= mod.base && address < mod.base + mod.size)
			{
				owner = &mod;
				break;
			}
		}

		if (owner != nullptr)
		{
			printf("  #%u: %s+0x%p\n", i, owner->name.c_str(), reinterpret_cast<void*>(address - owner->base));
		}
		else
		{
			printf("  #%u: 0x%p\n", i, frames[i]);
		}
	}
}

DWORD WINAPI HookThread(LPVOID lpThreadParameter)
{
	while (!hWnd) {
		hWnd = FindWindow("Valve001", NULL);
		if (!hWnd) {
			Sleep(1000); // Wait for 1 second before trying again
		}
	}

	oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

	if (!g_bUseOriginalServer)
	{
		while (!g_dwClientBase) // wait for client module
		{
			g_dwClientBase = (DWORD)GetModuleHandle("client.dll");
			Sleep(500);
		}
		g_dwClientSize = GetModuleSize(GetModuleHandle("client.dll"));
		if (g_bClassicUI) // only with --classicui; without it client.dll is left completely untouched
			CSO_ApplyClassicUIPreset(); // installs the GetGameMode() proxy + whitelist + all byte patches

		while (!g_dwGameUIBase) // wait for gameui module
		{
			g_dwGameUIBase = (DWORD)GetModuleHandle("gameui.dll");
			Sleep(500);
		}
		g_dwGameUISize = GetModuleSize(GetModuleHandle("gameui.dll"));
		InlineHook((void*)(g_dwGameUIBase + 0x712000), Hook_VGUI_MessageBox_New, (void*&)g_pfnVGUI_MessageBox_New);

		{
			const char* SHOP_CASH_SIG = AY_OBFUSCATE("\x6A\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x8B\x8B\x00\x00\x00\x00\x6A\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x8B\x8B\x00\x00\x00\x00\x6A\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x5F\x5E\x5B\x8B\xE5\x5D\xC2");
			const char* SHOP_CASH_MASK = AY_OBFUSCATE("x?xxxx????xx????x?xxxx????xx????x?xxxx????xxxxxxx");

			DWORD find = NULL;
			find = FindPattern((PCHAR)SHOP_CASH_SIG, (PCHAR)SHOP_CASH_MASK, g_dwGameUIBase, g_dwGameUIBase + g_dwGameUISize);
			if (!find)
				MessageBox(NULL, "SHOP_CASH_SIG == NULL!!!", "Error", MB_OK);
			else
			{
				WriteMemory((void *)(find + 1), (BYTE *)"\x01", 1); // force "CashBG" panel to be visible
				WriteMemory((void *)(find + 17), (BYTE *)"\x01", 1); // force "CashLabel" panel to be visible
				WriteMemory((void *)(find + 33), (BYTE *)"\x01", 1); // force "CashAmount" panel to be visible
			}

			const char* SHOP_CASH2_SIG = AY_OBFUSCATE("\x6A\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x8B\x8B\x00\x00\x00\x00\x6A\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x8B\x8B\x00\x00\x00\x00\x6A\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x5E\x5B\x8B\xE5\x5D\xC2\x00\x00\xCC\xCC\xCC\xCC\x55\x8B\xEC\x56\xFF\x75\x00\x8B\xF1\xE8");
			const char* SHOP_CASH2_MASK = AY_OBFUSCATE("x?xxxx????xx????x?xxxx????xx????x?xxxx????xxxxxx??xxxxxxxxxx?xxx");

			find = FindPattern((PCHAR)SHOP_CASH2_SIG, (PCHAR)SHOP_CASH2_MASK, g_dwGameUIBase, g_dwGameUIBase + g_dwGameUISize);
			if (!find)
				MessageBox(NULL, "SHOP_CASH2_SIG == NULL!!!", "Error", MB_OK);
			else
			{
				WriteMemory((void *)(find + 1), (BYTE *)"\x01", 1); // force "CashBG" panel to be visible
				WriteMemory((void *)(find + 17), (BYTE *)"\x01", 1); // force "CashLabel" panel to be visible
				WriteMemory((void *)(find + 33), (BYTE *)"\x01", 1); // force "CashAmount" panel to be visible
			}
		}

		InlineHook((void*)(g_dwGameUIBase + 0x5f7c30), Hook_CItemManagerDlgVer3_SetVisible, (void*&)g_pfnCItemManagerDlgVer3_SetVisible);

		if (g_pEngine)
		{
			g_pChattingManager = g_pEngine->GetChatManager();
			if (!g_pChattingManager)
				MessageBox(NULL, "g_pChattingManager == NULL!!!", "Error", MB_OK);
		}

		CreateInterfaceFn gameui_factory = CaptureFactory("gameui.dll");
		CreateInterfaceFn vgui2_factory = CaptureFactory("vgui2.dll");
		g_pGameUI = (IGameUI *)(CaptureInterface(gameui_factory, GAMEUI_INTERFACE_VERSION));
		g_pPanel = (vgui::IPanel *)(CaptureInterface(vgui2_factory, VGUI_PANEL_INTERFACE_VERSION));
		VFTHook(g_pGameUI, 0, 7, GameUI_RunFrame, (void *&)g_pfnGameUI_RunFrame);

		{
			DWORD find = NULL;
			find = FindPattern(CGAMEUI_PANELPOS_SIG_CSNZ, CGAMEUI_PANELPOS_MASK_CSNZ, g_dwGameUIBase, g_dwGameUIBase + g_dwGameUISize, 1);
			if (!find)
				MessageBox(NULL, "CGameUI_PanelPosition_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				// Restore webm background in main menu
				BYTE patch[] = {0x02};
				WriteMemory((void *)find, patch, sizeof(patch));
			}

			find = FindPattern(CGAMEUI_PANELPOS2_SIG_CSNZ, CGAMEUI_PANELPOS2_MASK_CSNZ, g_dwGameUIBase, g_dwGameUIBase + g_dwGameUISize, 1);
			if (!find)
				MessageBox(NULL, "CGameUI_PanelPosition2_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				// Restore webm background in main menu
				BYTE patch[] = {0x02};
				WriteMemory((void *)find, patch, sizeof(patch));
			}
		}

		while (!g_dwMpBase) // wait for mp.dll module
		{
			g_dwMpBase = (DWORD)GetModuleHandle("mp.dll");
			Sleep(500);
		}
		g_dwMpSize = GetModuleSize(GetModuleHandle("mp.dll"));

		{
			DWORD pushStr = 0;
			DWORD patchAddr = 0;
			BYTE patch[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

			// NOP IsDedi() function to load allstar Skill csv
			pushStr = FindPush(g_dwMpBase, g_dwMpBase + g_dwMpSize, (PCHAR)("Failed to Open AllStar_Skill-Dedi Table"));
			if (!pushStr)
				MessageBox(NULL, "AllStar_Skill_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				patchAddr = pushStr - 0x1B;
				WriteMemory((void *)patchAddr, (BYTE *)patch, sizeof(patch));
			}

			// NOP IsDedi() function to load allstar Status csv
			pushStr = FindPush(g_dwMpBase, g_dwMpBase + g_dwMpSize, (PCHAR)("Failed to Open AllStar_Status-Dedi Table"));
			if (!pushStr)
				MessageBox(NULL, "AllStar_Status_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				patchAddr = pushStr - 0x1E;
				WriteMemory((void *)patchAddr, (BYTE *)patch, sizeof(patch));
			}

			// NOP IsDedi() function to spawn zsht_item_box and zbsitem
			pushStr = FindPush(g_dwMpBase, g_dwMpBase + g_dwMpSize, (PCHAR)("zsht_item_box"), 3);
			if (!pushStr)
				MessageBox(NULL, "ZBS_ZSHT_ItemBox_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				patchAddr = pushStr - 0x9ED;
				WriteMemory((void *)patchAddr, (BYTE *)patch, sizeof(patch));
			}

			// Fix mp_freezetime for modes that start with 20 seconds
			pushStr = FindPush(g_dwMpBase, g_dwMpBase + g_dwMpSize, (PCHAR)("monster_spawn_point"), 3);
			if (!pushStr)
				MessageBox(NULL, "g_pFreezeTime == NULL!!!", "Error", MB_OK);
			else
			{
				g_pFreezeTime = *(float **)(pushStr + 0x13);

				DWORD find = FindPattern(CMAPENTITYMANAGER_SIG_CSNZ, CMAPENTITYMANAGER_MASK_CSNZ, g_dwMpBase, g_dwMpBase + g_dwMpSize, NULL);
				if (!find)
					MessageBox(NULL, "CMapEntityManager == NULL!!!", "Error", MB_OK);
				else
					InlineHook((void *)find, Hook_CMapEntityManager, (void *&)g_pfnCMapEntityManager);
			}

			// Commented out so can develop on HLDS
			// LoadLibraryA(AY_OBFUSCATE("CSOFix.dll"));
		}
	}

	return TRUE;
}

// filesystem_nar.dll: ComputeHash(path, flags, ...) returns XXH32 (seed 0) of the whole file; since the EoS update
// it maps the file itself, but only when the machine has >= 16 GB of RAM. Below that it still reads the whole file
// into a std::vector, which throws std::bad_alloc on big files on a thread-pool worker (nothing catches it ->
// terminate). Our ComputeHash always maps the file instead and never lets an exception escape.
static uint32_t XXH32(const uint8_t* p, size_t len, uint32_t seed)
{
    const uint32_t P1 = 2654435761U, P2 = 2246822519U, P3 = 3266489917U, P4 = 668265263U, P5 = 374761393U;
    auto rotl = [](uint32_t x, int r) { return (x << r) | (x >> (32 - r)); };
    auto rd = [](const uint8_t* q) { uint32_t v; memcpy(&v, q, 4); return v; };

    const uint8_t* end = p + len;
    uint32_t h;

    if (len >= 16)
    {
        const uint8_t* limit = end - 16;
        uint32_t v1 = seed + P1 + P2, v2 = seed + P2, v3 = seed, v4 = seed - P1;
        do
        {
            v1 = rotl(v1 + rd(p) * P2, 13) * P1; p += 4;
            v2 = rotl(v2 + rd(p) * P2, 13) * P1; p += 4;
            v3 = rotl(v3 + rd(p) * P2, 13) * P1; p += 4;
            v4 = rotl(v4 + rd(p) * P2, 13) * P1; p += 4;
        } while (p <= limit);
        h = rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);
    }
    else
    {
        h = seed + P5;
    }

    h += (uint32_t)len;

    while (p + 4 <= end)
    {
        h = rotl(h + rd(p) * P3, 17) * P4;
        p += 4;
    }
    while (p < end)
    {
        h = rotl(h + (*p++) * P5, 11) * P1;
    }

    h ^= h >> 15; h *= P2;
    h ^= h >> 13; h *= P3;
    h ^= h >> 16;
    return h;
}

static bool SafeXXH32(const uint8_t* data, size_t len, uint32_t* out)
{
    // a file that shrinks while it is mapped raises an in-page error; report it instead of crashing the worker
    __try {
        *out = XXH32(data, len, 0);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

CreateHook(__cdecl, int, ComputeHash, const char* lpFileName, [[maybe_unused]] int flags, [[maybe_unused]] int dwSize) {
    return g_pfnComputeHash(lpFileName, flags, dwSize);
}

void Init(HMODULE hEngineModule, HMODULE hFileSystemModule)
{
	g_hEngineModule = hEngineModule;
	g_dwEngineBase = GetModuleBase(g_hEngineModule);
	g_dwEngineSize = GetModuleSize(g_hEngineModule);

	g_dwFileSystemBase = GetModuleBase(hFileSystemModule);
	g_dwFileSystemSize = GetModuleSize(hFileSystemModule);

	if (CommandLine()->CheckParm("-debug") || CommandLine()->CheckParm("-dev") || CommandLine()->CheckParm("+developer 1") || CommandLine()->CheckParm("-developer"))
	{
		CreateDebugConsole();

		void *find = (void *)FindPattern(LOGTOERRORLOG_SIG_CSNZ, LOGTOERRORLOG_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "LogToErrorLog == NULL!!!", "Error", MB_OK);
		else
			InlineHook(find, Hook_LogToErrorLog, (void *&)g_pfnLogToErrorLog);

		find = GetProcAddress(GetModuleHandle("kernel32.dll"), "OutputDebugStringA");
		if (!find)
			MessageBox(NULL, "OutputDebugStringA == NULL!!!", "Error", MB_OK);
		else
			InlineHook(find, Hook_OutputDebugStringAHook, (void *&)g_pfnOutputDebugStringAHook);
	}

	printf("Init()\n");

	const char *port;
	const char *ip;

	if (CommandLine()->CheckParm("-ip", &ip) && ip)
	{
		strncpy(g_pServerIP, ip, sizeof(g_pServerIP));
	}
	else
	{
		strncpy(g_pServerIP, DEFAULT_IP, sizeof(DEFAULT_IP));
	}

	if (CommandLine()->CheckParm("-port", &port) && port)
	{
		strncpy(g_pServerPort, port, sizeof(g_pServerPort));
	}
	else
	{
		strncpy(g_pServerPort, DEFAULT_PORT, sizeof(DEFAULT_PORT));
	}

	const char *login;
	const char *password;

	if (CommandLine()->CheckParm("-login", &login) && login && CommandLine()->CheckParm("-password", &password) && password)
	{
		strncpy(g_pLogin, login, sizeof(g_pLogin));
		strncpy(g_pPassword, password, sizeof(g_pPassword));

		g_bDisableAuthUI = true;
	}

#ifdef _DEBUG
	g_bUseOriginalServer = CommandLine()->CheckParm("-useoriginalserver");
	g_bDumpMetadata = CommandLine()->CheckParm("-dumpmetadata");
	g_bIgnoreMetadata = CommandLine()->CheckParm("-ignoremetadata");
	g_bDumpAll = CommandLine()->CheckParm("-dumpall");
	g_bDisableAuthUI = CommandLine()->CheckParm("-disableauthui");
	g_bUseSSL = CommandLine()->CheckParm("-usessl");
	g_bWriteMetadata = CommandLine()->CheckParm("-writemetadata");
	g_bLoadDediCsvFromFile = CommandLine()->CheckParm("-loaddedicsvfromfile");
	g_bNoNGHook = CommandLine()->CheckParm("-nonghook");
	g_bNoZBSElitePatch = CommandLine()->CheckParm("-nozbselite");
	g_bFixWeapon = CommandLine()->CheckParm("-fixweapon");
	g_bGoldenZombie = CommandLine()->CheckParm("-goldenzombie");

	g_bClassicUI = CommandLine()->CheckParm("--classicui") != NULL;
	printf("g_pServerIP = %s, g_pServerPort = %s\n", g_pServerIP, g_pServerPort);
#endif
}

std::string GetExecutableName()
{
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	std::string exeName = std::filesystem::path(exePath).filename().string();
	return exeName;
}

void Hook(HMODULE hEngineModule, HMODULE hFileSystemModule)
{
	Init(hEngineModule, hFileSystemModule);

	DWORD find = NULL;
	void *dummy = NULL;

	// filesystem_nar.dll: replace ComputeHash so big files can not throw std::bad_alloc on a pool worker (see Hook_ComputeHash)
	#define COMP_HASH_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x83\xEC\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\x00\x0F\xB6\x45\x00\x53\x6A\x00\x83\xF0\x01\xC1\xE0\x1D\x05\x01\x00\x00\x08\x50\x6A\x03\x6A\x00\x6A\x01\x68\x00\x00\x00\x80\xFF\x75\x00\xFF\x15")
	#define COMP_HASH_MASK_CSNZ AY_OBFUSCATE("xxxxx?x????xxxx?xxx?xxxxxxxxxxxxxxxxxxxxxxxxxxxx?xx")

	find = FindPattern((PCHAR)COMP_HASH_SIG_CSNZ, (PCHAR)COMP_HASH_MASK_CSNZ, g_dwFileSystemBase, g_dwFileSystemBase + g_dwFileSystemSize);
	if (!find)
		MessageBox(NULL, "COMP_HASH == NULL!!!", "Error", MB_OK);
	else
		InlineHook((void *)find, Hook_ComputeHash, (void *&)g_pfnComputeHash);

	#define NETWORKMANAGER_SIG_CSNZ AY_OBFUSCATE("\xA3\x00\x00\x00\x00\x8B\x4D\x00\x64\x89\x0D\x00\x00\x00\x00\x59\x8B\xE5\x5D\xC3\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x8B\x4D\x00\x64\x89\x0D\x00\x00\x00\x00\x59\x8B\xE5\x5D\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x55")
	#define NETWORKMANAGER_MASK_CSNZ AY_OBFUSCATE("x????xx?xxx????xxxxxxx????????xx?xxx????xxxxxxxxxxxxxxxx")

	find = FindPattern((PCHAR)NETWORKMANAGER_SIG_CSNZ, (PCHAR)NETWORKMANAGER_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
	if (!find)
		MessageBox(NULL, "NetworkManager == NULL!!!", "Error", MB_OK);
	else
	{
		DWORD** pNetworkManager = *(DWORD ***)(find + 1);
		g_pNetworkManager = pNetworkManager;
	}

	#define DISCONNECTCLIENT_SIG_CSNZ AY_OBFUSCATE("\x55\x8B\xEC\x56\x8B\xF1\x83\x7E\x00\x00\x0F\x84\x00\x00\x00\x00\x80\x7D")
	#define DISCONNECTCLIENT_MASK_CSNZ AY_OBFUSCATE("xxxxxxxx??xx????xx")

	find = FindPattern((PCHAR)DISCONNECTCLIENT_SIG_CSNZ, (PCHAR)DISCONNECTCLIENT_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
	if (!find)
		MessageBox(NULL, "DisconnectClient == NULL!!!", "Error", MB_OK);
	else
		g_pfnDisconnectClient = (DisconnectClient)find;

	if (!g_bNoNGHook)
	{
		find = FindPattern(NGCLIENT_INIT_SIG_CSNZ, NGCLIENT_INIT_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, 0x11);
		if (!find)
			MessageBox(NULL, "NGClient_Init == NULL!!!", "Error", MB_OK);
		else
			InlineHookFromCallOpcode((void *)find, NGClient_Return1, dummy, dummy);

		find = FindPattern(NGCLIENT_QUIT_SIG_CSNZ, NGCLIENT_QUIT_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "NGClient_Quit == NULL!!!", "Error", MB_OK);
		else
			InlineHook((void *)find, NGClient_Return17238, dummy);

		find = FindPattern(PACKET_HACK_SEND_SIG_CSNZ, PACKET_HACK_SEND_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "Packet_Hack_Send == NULL!!!", "Error", MB_OK);
		else
		{
			InlineHookFromCallOpcode((void *)find, NGClient_Return17238, dummy, dummy);
			InlineHookFromCallOpcode((void *)(find + 0x5), NGClient_Return17238, dummy, dummy);
		}

		find = FindPattern(PACKET_HACK_PARSE_SIG_CSNZ, PACKET_HACK_PARSE_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "Packet_Hack_Parse == NULL!!!", "Error", MB_OK);
		else
			InlineHook((void *)find, Hook_Packet_Hack_Parse, dummy);
	}

	if (!g_bUseOriginalServer)
	{
		IATHook(g_hEngineModule, "nxgsm.dll", "InitializeGameLogManagerA", NXGSM_Dummy, dummy);
		IATHook(g_hEngineModule, "nxgsm.dll", "WriteStageLogA", NXGSM_WriteStageLogA, dummy);
		IATHook(g_hEngineModule, "nxgsm.dll", "WriteErrorLogA", NXGSM_WriteErrorLogA, dummy);
		IATHook(g_hEngineModule, "nxgsm.dll", "FinalizeGameLogManager", NXGSM_Dummy, dummy);
		IATHook(g_hEngineModule, "nxgsm.dll", "SetUserSN", NXGSM_Dummy, dummy);

		find = FindPattern(SOCKETMANAGER_SIG_CSNZ23, SOCKETMANAGER_MASK_CSNZ23, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "SocketManagerConstructor == NULL!!!", "Error", MB_OK);
		else
			InlineHook((void *)find, Hook_SocketManagerConstructor, (void *&)g_pfnSocketManagerConstructor);

		find = FindPattern(SERVERCONNECT_SIG_CSNZ2019, SERVERCONNECT_MASK_CSNZ2019, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "ServerConnect == NULL!!!", "Error", MB_OK);
		else
			InlineHookFromCallOpcode((void *)find, Hook_ServerConnect, (void *&)g_pfnServerConnect, dummy);

		find = FindPattern(HOLEPUNCH_SETSERVERINFO_SIG_CSNZ, HOLEPUNCH_SETSERVERINFO_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "HolePunch_SetServerInfo == NULL!!!", "Error", MB_OK);
		else
			InlineHook((void *)find, Hook_HolePunch_SetServerInfo, (void *&)g_pfnHolePunch_SetServerInfo);

		find = FindPattern(HOLEPUNCH_GETUSERSOCKETINFO_SIG_CSNZ, HOLEPUNCH_GETUSERSOCKETINFO_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "HolePunch_GetUserSocketInfo == NULL!!!", "Error", MB_OK);
		else
			InlineHook((void *)find, Hook_HolePunch_GetUserSocketInfo, (void *&)g_pfnHolePunch_GetUserSocketInfo);

		{
			// NOP dedi check on Zombie Skills
			find = FindPush(g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, (PCHAR)("resource/zombi/ZombieSkillProperty_Dedi/ZombieSkillProperty_Crazy.csv"));
			if (!find)
				MessageBox(NULL, "ZombieSkillProperty_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				find -= 0x30;
				BYTE patch[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xEB, 0x07};
				WriteMemory((void *)find, (BYTE *)patch, sizeof(patch));
			}

			// NOP dedi check on Fire Bomb
			find = FindPush(g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, (PCHAR)("resource/zombi/FireBombOption_Dedi.csv"));
			if (!find)
				MessageBox(NULL, "FireBombOption_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				find -= 0x8;
				BYTE patch2[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
				WriteMemory((void *)find, (BYTE *)patch2, sizeof(patch2));
			}

			find = FindPattern(CREATESTRINGTABLE_SIG_CSNZ, CREATESTRINGTABLE_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
			if (!find)
				MessageBox(NULL, "CreateStringTable == NULL!!!", "Error", MB_OK);
			else
			{
				InlineHook((void *)find, Hook_CreateStringTable, (void *&)g_pfnCreateStringTable);

				DWORD parseCsvCallAddr = (DWORD)find + 0x71 + 1; // 0x71
				g_pfnParseCSV = (tParseCSV)(parseCsvCallAddr + 4 + *(DWORD *)parseCsvCallAddr);
			}

			find = FindPattern(LOADJSON_SIG_CSNZ, LOADJSON_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
			if (!find)
				MessageBox(NULL, "LoadJson == NULL!!!", "Error", MB_OK);
			else
				InlineHook((void *)find, Hook_LoadJson, (void *&)g_pfnLoadJson);

			// patch socket
			find = FindPush(g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, (PCHAR)("SocketManager - Max retry reached!!! Server Connection failed \n"));
			if (!find)
				MessageBox(NULL, "Socket_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				find -= 0xE;
				BYTE patch[] = {0x01};
				WriteMemory((void *)find, (BYTE *)patch, sizeof(patch));
			}

			// patch "CheckServerAlive failed"
			// bisa bikin run lang lain di client ini
			find = FindPattern(CONNECTSERVER_SIG_CSNZ, CONNECTSERVER_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
			if (!find)
				MessageBox(NULL, "CheckServerAlive_Patch == NULL!!!", "Error", MB_OK);
			else
			{
				find += 0xFC;
				BYTE patch[] = {0x85};
				WriteMemory((void *)find, (BYTE *)patch, sizeof(patch));
			}
		}
	}

	g_pEngine = (cl_enginefunc_t *)(PVOID) * (PDWORD)(FindPush(g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, (PCHAR)("ScreenFade")) + 0x0D);
	if (!g_pEngine)
		MessageBox(NULL, "g_pEngine == NULL!!!", "Error", MB_OK);
	else {
		DWORD staticAddress = (DWORD)g_pEngine->Pbuf_AddText - g_dwEngineBase;
		g_pEngine->Pbuf_AddText = Pbuf_AddText;
	}

	if (g_bDumpAll || g_bDumpMetadata || g_bWriteMetadata || g_bIgnoreMetadata)
	{
		find = FindPattern(PACKET_METADATA_PARSE_SIG_CSNZ, PACKET_METADATA_PARSE_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "Packet_Metadata_Parse == NULL!!!", "Error", MB_OK);
		else
		{
			InlineHook((void *)find, Hook_Packet_Metadata_Parse, (void *&)g_pfnPacket_Metadata_Parse);
			if (g_pEngine)
				g_pEngine->pfnAddCommand("metadata_requestall", Metadata_RequestAll);
		}
	}

	find = FindPattern(READPACKET_SIG_CSNZ, READPACKET_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
	if (!find)
		MessageBox(NULL, "ReadPacket == NULL!!!", "Error", MB_OK);
	else
		InlineHookFromCallOpcode((void *)find, Hook_ReadPacket, (void *&)g_pfnReadPacket, dummy);

	find = FindPattern(WRITEPACKET_SIG_CSNZ, WRITEPACKET_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
	if (!find)
		MessageBox(NULL, "WritePacket == NULL!!!", "Error", MB_OK);
	else
		InlineHook((void *)find, Hook_WritePacket, (void *&)g_pfnWritePacket);

	// Fix "error TS%d,%d (GetLastError 0x%x)"
	find = FindPattern(GETPECOMPILATIONTIME_SIG_CSNZ, GETPECOMPILATIONTIME_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
	if (!find)
		MessageBox(NULL, "GetPECompilationTime == NULL!!!", "Error", MB_OK);
	else
		InlineHook((void *)find, Hook_GetPECompilationTime, (void *&)g_pfnGetPECompilationTime);

	if (!g_bUseOriginalServer)
	{
		find = FindPattern(ITEM_TO_WEAPONINDEX_SIG_CSNZ, ITEM_TO_WEAPONINDEX_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "ItemToWeaponIndex == NULL!!!", "Error", MB_OK);
		else
			InlineHook((void *)find, Hook_ItemToWeaponIndex, (void *&)g_pfnItemToWeaponIndex);

		find = FindPattern(GAMEMODE_OBJECT_SIG_CSNZ, GAMEMODE_OBJECT_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "GameModeObject == NULL!!!", "Error", MB_OK);
		else
			g_dwGameModeObject = *(DWORD *)(find + 1);

		find = FindPattern(BUYMENU_REFRESH_SIG_CSNZ, BUYMENU_REFRESH_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "BuyMenuRefresh == NULL!!!", "Error", MB_OK);
		else
		{
			g_dwBuyMenuKeyOffset = *(DWORD *)(find + 34);
			if (g_dwGameModeObject)
				InlineHook((void *)find, Hook_BuyMenuRefresh, (void *&)g_pfnBuyMenuRefresh);
		}

		// guard the weapon info table lookup (see Hook_WeaponInfoLookup)
		find = FindPattern(WEAPONINFO_LOOKUP_SIG_CSNZ, WEAPONINFO_LOOKUP_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "WeaponInfoLookup == NULL!!!", "Error", MB_OK);
		else
		{
			g_pWeaponInfoTable = *(DWORD **)(find + 32);
			InlineHook((void *)find, Hook_WeaponInfoLookup, (void *&)g_pfnWeaponInfoLookup);
		}
	}

	// patch 100 fps limit
	find = FindPattern(FPS_PATCH_SIG_CSNZ, FPS_PATCH_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
	if (!find)
		MessageBox(NULL, "100Fps_Patch == NULL!!!", "Error", MB_OK);
	else
	{
		BYTE patch[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		WriteMemory((void *)find, (BYTE *)patch, sizeof(patch));
	}

	if (!g_bUseOriginalServer && g_bGoldenZombie)
	{
		find = FindPattern(GOLDENZOMBIEFORLAUNCHER_SIG_CSNZ, GOLDENZOMBIEFORLAUNCHER_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "GoldenZombieForLauncher_Patch == NULL!!!", "Error", MB_OK);
		else
		{
			{
				DWORD patchAddr = find + 0x31;
				BYTE patch[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
				WriteMemory((void *)patchAddr, (BYTE *)patch, sizeof(patch));
			}
			{
				DWORD patchAddr = find + 0x3E;
				BYTE patch[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
				WriteMemory((void *)patchAddr, (BYTE *)patch, sizeof(patch));
			}
		}
	}

	if (!g_bUseOriginalServer && !g_bUseSSL)
	{
		// hook GetSSLProtocolName to make Crypt work
		find = FindPattern(GETSSLPROTOCOLNAME_SIG_CSNZ, GETSSLPROTOCOLNAME_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "GetSSLProtocolName == NULL!!!", "Error", MB_OK);
		else
			InlineHookFromCallOpcode((void *)find, Hook_GetSSLProtocolName, dummy, dummy);

		// hook SocketConstructor to create ctx objects
		find = FindPattern(SOCKETCONSTRUCTOR_SIG_CSNZ, SOCKETCONSTRUCTOR_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "SocketConstructor == NULL!!!", "Error", MB_OK);
		else
			InlineHookFromCallOpcode((void *)find, Hook_SocketConstructor, (void *&)g_pfnSocketConstructor, dummy);

		find = FindPattern(EVP_CIPHER_CTX_NEW_SIG_CSNZ, EVP_CIPHER_CTX_NEW_MASK_CSNZ, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize, NULL);
		if (!find)
			MessageBox(NULL, "EVP_CIPHER_CTX_new == NULL!!!", "Error", MB_OK);
		else
		{
			DWORD dwCreateCtxAddr = find + 1;
			g_pfnEVP_CIPHER_CTX_new = (tEVP_CIPHER_CTX_new)(dwCreateCtxAddr + 4 + *(DWORD *)dwCreateCtxAddr);
		}
	}

	const char* notice_pattern = AY_OBFUSCATE("\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x00\xE8\x00\x00\x00\x00\x8B\x08\x8B\x51\x00\x8D\x8D\x00\x00\x00\x00\x51\x8B\xC8\xFF\xD2\x85\xC0");
	const char* notice_mask = AY_OBFUSCATE("x????xx????xx?x????xxxx?xx????xxxxxxx");

	find = FindPattern((PCHAR)notice_pattern, (PCHAR)notice_mask, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
	if (!find)
		MessageBox(NULL, "#CSO_Warning_%s Patch == NULL!!!", "Error", MB_OK);
	else
	{
		static const char* new_fmt = "#%s\0";
		WriteMemory((void*)(find + 1), (BYTE*)&new_fmt, sizeof(new_fmt));
	}

	find = FindPattern((PCHAR)CSONWRAPPER_LOGIN_SIG, CSONWRAPPER_LOGIN_MASK, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
	if (!find)
		MessageBox(NULL, "CSONWrapper_Login Patch == NULL!!!", "Error", MB_OK);
	else
		InlineHook((void*)find, Hook_CSONWrapper_Login, (void*&)g_pfnCSONWrapper_Login);

	const char* bypass_region_check = AY_OBFUSCATE("\x85\xC0\x0F\x84\x00\x00\x00\x00\x83\xF8\x00\x0F\x84\x00\x00\x00\x00\x83\xF8\x00\x0F\x85");
	const char* bypass_region_check_mask = AY_OBFUSCATE("xxxx????xx?xx????xx?xx");

	find = FindPattern((PCHAR)bypass_region_check, (PCHAR)bypass_region_check_mask, g_dwEngineBase, g_dwEngineBase + g_dwEngineSize);
	if (!find)
		MessageBox(NULL, "Bypass Region Check Patch == NULL!!!", "Error", MB_OK);
	else
	{
		BYTE patch1[] = { 0x90, 0x90, 0x90 }; // NOP the test instruction
		WriteMemory((void*)(find), patch1, sizeof(patch1));
		BYTE patch2[] = { 0xB8, 0x03, 0x00, 0x00, 0x00 }; // mov eax, 1
		WriteMemory((void*)(find + sizeof(patch1)), patch2, sizeof(patch2));
	}

	// create thread to wait for other modules
	CreateThread(NULL, 0, HookThread, NULL, 0, 0);
}

void Unhook()
{
	FreeAllHook();
}