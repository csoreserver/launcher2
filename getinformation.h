#pragma once
#include <Windows.h>

void GetInformation_Run(
    HMODULE hEngineModule,
    HMODULE hFileSystemModule,
    const char* cmdLine,
    const char* szNewCommandParams);