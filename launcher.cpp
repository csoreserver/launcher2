#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include "HLSDK/common/interface.h"
#include "IEngine.h"
#include "ICommandLine.h"
#include "IRegistry.h"
#include "IFileSystem.h"
#include "sys.h"
#include "hook.h"
#include <stdio.h>
#include "dump.h"
#include "hookutils.h"
#include "obfuscate.h"
#include "getinformation.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>

IFileSystem* g_pFileSystem;
int g_iMaxGoldenZombie = 1;
#define DEFAULT_MAXGOLDENZOMBIE "1"

HINTERFACEMODULE LoadFilesystemModule(void)
{
	HINTERFACEMODULE hModule = Sys_LoadModule("filesystem_nar.dll");

	if (!hModule)
	{
		MessageBox(NULL, "Could not load filesystem dll.\nFileSystem crashed during construction.", "Fatal Error", MB_ICONERROR);
		return NULL;
	}

	return hModule;
}

std::string GetExecutableName();

// Runs one launcher iteration under SEH. Kept separate from WinMain because __try
// can't be used in a function that needs C++ object unwinding.
// Returns true if the loop should continue. InitCrashDump's handler may be hijacked, so we write the dump ourselves.
static bool RunIterationLogged(const std::function<bool()>& iteration)
{
	try
	{
		return iteration();
	}
	catch (const std::exception& e)
	{
		// Log the message, then rethrow so the SEH filter below still captures the stack/dump.
		char msg[512];
		_snprintf(msg, sizeof(msg), "Unhandled C++ exception: %s\n", e.what());
		msg[sizeof(msg) - 1] = 0;
		OutputDebugString(msg);
		fputs(msg, stderr);
		throw;
	}
}

static bool RunIterationGuarded(const std::function<bool()>& iteration)
{
	__try
	{
		return RunIterationLogged(iteration);
	}
	__except (ExceptionFilter(GetExceptionInformation()))
	{
		char msg[256];
		_snprintf(msg, sizeof(msg), "The game has encountered a fatal error and needs to close.\nException code: 0x%08lX\n\nA crash report has been saved.", (unsigned long)GetExceptionCode());
		msg[sizeof(msg) - 1] = 0;
		MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR | MB_OK);
		return false;
	}
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));
	InitCrashDump();

	static char szNewCommandParams[2048];
	HANDLE hObject = NULL;

	VirtualAlloc(0, 0x400u, 0x2000u, 4u);

	HANDLE ProcessHeaps[1025];
	DWORD NumberOfHeaps = GetProcessHeaps(1024, ProcessHeaps);
	DWORD HeapInformation = 1;
	for (int i = 0; i < NumberOfHeaps; i++)
	{
		if (HeapSetInformation(ProcessHeaps[i], HeapCompatibilityInformation, &HeapInformation, 4u))
		{
			printf("The low-fragmentation heap has been enabled.\n");
		}
		else
		{
			printf("Failed to enable the low-fragmentation heap, %d.\n", GetLastError());
		}
	}

	CommandLine()->CreateCmdLine(GetCommandLine());

#ifndef _DEBUG
	BOOL(*IsDebuggerPresent)(void) = (BOOL(*)(void))GetProcAddress(GetModuleHandle("kernel32.dll"), "IsDebuggerPresent");

	if (!IsDebuggerPresent() && CommandLine()->CheckParm("-nomutex") == NULL)
	{
		hObject = CreateMutex(NULL, FALSE, "NexonCSOMutex");

		DWORD dwStatus = WaitForSingleObject(hObject, 0);

		if (dwStatus && dwStatus != WAIT_ABANDONED)
		{
			MessageBox(NULL, "Could not launch game.\nOnly one instance of this game can be run at a time.", "Error", MB_ICONERROR);
			return 0;
		}
	}
#endif

	WSAData WSAData;
	WSAStartup(0x202, &WSAData);

	registry->Init();

	char szFileName[MAX_PATH];
	Sys_GetExecutableName(szFileName, sizeof(szFileName));
	char* szExeName = strrchr(szFileName, '\\') + 1;

	if (_stricmp(szExeName, "hl.exe") && CommandLine()->CheckParm("-game") == NULL)
	{
		szExeName[strlen(szExeName) - 4] = '\0';
		CommandLine()->AppendParm("-game", szExeName);
	}

	static char szGameName[32] = { 0 };
	const char* pszGameName = NULL;
	const char* szGameStr = CommandLine()->CheckParm("-game", &pszGameName);
	const char* maxGoldenZombie;

	strncpy(szGameName, (szGameStr) ? pszGameName : "valve", sizeof(szGameName) - 1);
	szGameName[sizeof(szGameName) - 1] = 0;

	if (szGameStr && !_strnicmp(&szGameStr[6], "czero", 5))
		CommandLine()->AppendParm("-forcevalve", NULL);

	if (CommandLine()->CheckParm("-lang") == NULL)
		CommandLine()->AppendParm("-lang", "na_id"); 	// the game won't load without this line

	if (CommandLine()->CheckParm("-maxgoldenzombie", &maxGoldenZombie) == NULL)
	{
		CommandLine()->AppendParm("-maxgoldenzombie", DEFAULT_MAXGOLDENZOMBIE);
		g_iMaxGoldenZombie = atoi(DEFAULT_MAXGOLDENZOMBIE);
	}
	else if (maxGoldenZombie)
		g_iMaxGoldenZombie = atoi(maxGoldenZombie);

	bool devmodeactivated = CommandLine()->CheckParm("-debug") || CommandLine()->CheckParm("-dev") || CommandLine()->CheckParm("+developer 1") || CommandLine()->CheckParm("-developer");

	auto runIteration = [&]() -> bool
	{
		HINTERFACEMODULE hFileSystem = LoadFilesystemModule();

		if (!hFileSystem)
			return false;

		CreateInterfaceFn fsCreateInterface = (CreateInterfaceFn)Sys_GetFactory(hFileSystem);
		g_pFileSystem = (IFileSystem*)fsCreateInterface(FILESYSTEM_INTERFACE_VERSION, NULL);
		g_pFileSystem->Mount();
		g_pFileSystem->AddSearchPath(Sys_GetLongPathName(), "BIN");

		char fDir[MAX_PATH];
		sprintf(fDir, "%s\\%s", Sys_GetLongPathNameWithoutBin(), "Data");
		g_pFileSystem->_AddResourcePath(fDir);

		const char* pszEngineDLL = "hw.dll";
		int iResult = ENGINE_RESULT_NONE;

		registry->WriteString("EngineDLL", pszEngineDLL);

		szNewCommandParams[0] = 0;

		IEngine* engineAPI = NULL;
		HINTERFACEMODULE hEngine;
		bool bUseBlobDLL = false;

		hEngine = Sys_LoadModule(pszEngineDLL);
		if (!hEngine)
		{
			static char msg[512];
			wsprintf(msg, "Could not load engine : %s.", pszEngineDLL);
			MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
			ExitProcess(0);
		}

		if (CommandLine()->CheckParm("-getfiletimestamp", NULL))
		{
			iResult = ENGINE_RESULT_NONE;
			CommandLine()->AppendParm("+developer", "1");

			Hook((HMODULE)hEngine, (HMODULE)hFileSystem);
			GetInformation_Run((HMODULE)hEngine, (HMODULE)hFileSystem, CommandLine()->GetCmdLine(), szNewCommandParams);
			Unhook();
		}
		else {
			CreateInterfaceFn engineCreateInterface = (CreateInterfaceFn)Sys_GetFactory(hEngine);
			engineAPI = (IEngine*)engineCreateInterface(VENGINE_LAUNCHER_API_VERSION, NULL);

			if (!engineCreateInterface || !engineAPI)
				Sys_FreeModule(hEngine);

			if (engineAPI)
			{
				Hook((HMODULE)hEngine, (HMODULE)hFileSystem);			
				iResult = engineAPI->Run(hInstance, Sys_GetLongPathNameWithoutBin(), CommandLine()->GetCmdLine(), szNewCommandParams, Sys_GetFactoryThis(), Sys_GetFactory(hFileSystem));
				Unhook();

				Sys_FreeModule(hEngine);
			}
		}

		if (iResult == ENGINE_RESULT_NONE || iResult > ENGINE_RESULT_UNSUPPORTEDVIDEO)
			return false;

		bool bContinue = false;

		switch (iResult)
		{
		case ENGINE_RESULT_RESTART:
		{
			bContinue = true;
			break;
		}
		case ENGINE_RESULT_UNSUPPORTEDVIDEO:
		{
			bContinue = MessageBox(NULL, "The specified video mode is not supported.\n", "Video mode change failure", MB_OKCANCEL | MB_ICONWARNING) != IDOK;
			break;
		}
		}

		CommandLine()->RemoveParm("-sw");
		CommandLine()->RemoveParm("-startwindowed");
		CommandLine()->RemoveParm("-windowed");
		CommandLine()->RemoveParm("-window");
		CommandLine()->RemoveParm("-full");
		CommandLine()->RemoveParm("-fullscreen");
		CommandLine()->RemoveParm("-soft");
		CommandLine()->RemoveParm("-software");
		CommandLine()->RemoveParm("-gl");
		CommandLine()->RemoveParm("-d3d");
		CommandLine()->RemoveParm("-w");
		CommandLine()->RemoveParm("-width");
		CommandLine()->RemoveParm("-h");
		CommandLine()->RemoveParm("-height");
		CommandLine()->RemoveParm("-novid");
		CommandLine()->RemoveParm("+connect");

		if (strstr(szNewCommandParams, "-game"))
			CommandLine()->RemoveParm("-game");

		if (strstr(szNewCommandParams, "+load"))
			CommandLine()->RemoveParm("+load");

		CommandLine()->AppendParm(szNewCommandParams, NULL);

		g_pFileSystem->Unmount();

		Sys_FreeModule(hFileSystem);

		return bContinue;
	};

	while (RunIterationGuarded(runIteration))
	{
	}

	if (devmodeactivated)
	{
		// stupid whatever does
		printf("\nPress any key to exit...");
		getchar();
	}

	registry->Shutdown();

	if (hObject)
	{
		ReleaseMutex(hObject);
		CloseHandle(hObject);
	}

	WSACleanup();

	return 1;
}