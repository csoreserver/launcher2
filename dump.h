#pragma once

#include <windows.h>

void InitCrashDump();
LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS *exceptionPointers);