#pragma once

#include <windows.h>

struct hook_s
{
	void* pOldFuncAddr;
	void* pNewFuncAddr;
	void* pClass;
	int iTableIndex;
	int iFuncIndex;
	HMODULE hModule;
	const char* pszModuleName;
	const char* pszFuncName;
	void* pRawVTable;
	void* pOriginRawFunction;
	int iRawTableIndex;
	struct hook_s* pNext;
	void* pInfo;
};

typedef struct hook_s hook_t;

void FreeAllHook(void);

DWORD GetModuleBase(HMODULE hModule);
DWORD GetModuleSize(HMODULE hModule);

hook_t* FindInlineHooked(void* pOldFuncAddr);
hook_t* FindVFTHooked(void* pClass, int iTableIndex, int iFuncIndex);
hook_t* FindIATHooked(HMODULE hModule, const char* pszModuleName, const char* pszFuncName);
BOOL UnHook(hook_t* pHook);
hook_t* InlineHook(void* pOldFuncAddr, void* pNewFuncAddr, void*& pCallBackFuncAddr);
hook_t* InlineHookFromCallOpcode(void* pOldFuncAddr, void* pNewFuncAddr, void*& pCallBackFuncAddr, void*& pFuncAddr);
hook_t* RawVTFHook(void* pVTBL, int iFuncIndex, void* pNewFuncAddr, void*& pCallBackFuncAddr);
hook_t* VFTHook(void* pClass, int iTableIndex, int iFuncIndex, void* pNewFuncAddr, void*& pCallBackFuncAddr); // ultra
hook_t* IATHook(HMODULE hModule, const char* pszModuleName, const char* pszFuncName, void* pNewFuncAddr, void*& pCallBackFuncAddr);
hook_t* IATHookOrdinal(HMODULE hModule, const char* pszModuleName, int ordinal, void* pNewFuncAddr, void*& pCallBackFuncAddr);
void *GetClassFuncAddr(...);
void WriteDWORD(void *pAddress, DWORD dwValue);
DWORD ReadDWORD(void *pAddress);
DWORD WriteMemory(void *pAddress, BYTE *pData, DWORD dwDataSize);
DWORD ReadMemory(void *pAddress, BYTE *pData, DWORD dwDataSize);
void FillMemoryWithNOP(void *pAddress, DWORD dwSize);
DWORD FindPattern(PCHAR pattern, PCHAR mask, DWORD start, DWORD end, DWORD offset = 0);
DWORD FindPattern(PCHAR pattern, DWORD patternLength, DWORD start, DWORD end, DWORD offset = 0, DWORD refNumber = 1);
DWORD FindPush(DWORD start, DWORD end, PCHAR Message, DWORD refNumber = 1);

#define CreateHook(funcType, returnType, funcName, ...) \
returnType (funcType* g_pfn##funcName)(__VA_ARGS__); \
returnType funcType Hook_##funcName(__VA_ARGS__)

#define CreateHookClassType(returnType,funcName,classType, ...) \
returnType (__thiscall* g_pfn##funcName)(classType*ptr, __VA_ARGS__); \
returnType __fastcall Hook_##funcName(classType*ptr, int reg, __VA_ARGS__)

#define CreateHookClass(returnType, funcName, ...) CreateHookClassType(returnType, funcName, void, __VA_ARGS__)