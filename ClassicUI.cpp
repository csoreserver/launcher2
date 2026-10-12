// Preset "classicUI" for client.dll: GetGameMode() proxy + call-site whitelist + byte patches.
// Everything is applied by CSO_ApplyClassicUIPreset(); see the comment above that function.

#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#include "ClassicUI.h"
#include "hookutils.h"

// from hook.cpp
extern DWORD g_dwClientBase;
void PrintStackTrace();

// ClassicUI only can be enabled on this mode
// if not use default UI (-1) for all other modes
const int allowedGameModes[] = {
	45, // ZombieZ
	14, // ZombieHero3
	58, // ZombieHero3Classic
};

// RVA of dword_3A83BE8 relative to client.dll's IDA imagebase (0x1900000).
#define CLIENT_GAMEMODE_ACCESSOR_RVA 0x2183BE8
#define GAMEMODE_VTABLE_RANGE 0x1000
#define GAMEMODE_VTABLE_SLOTS (GAMEMODE_VTABLE_RANGE / 4)

DWORD g_dwGameModeRealObj = 0;        // current real gamemode-manager object pointer (refreshed every accessor call)
DWORD g_dwGameModeRealAccessor = 0;   // original function that was in dword_3688518 before we hooked it
int   g_iGameModeOverride = 21;       // 21 = classic CS HUD everywhere (only whitelisted call sites see it); -1 = passthrough (transparent); anything else forces GetGameMode() to that value, client.dll-side only

static char szGameModeTrapCaption[] = "Error";
static char szGameModeTrapText[] = "CSO_FakeGameModeObj: unexpected vtable slot called (not covered by the sweep)!";

// Any vtable slot client.dll calls that we didn't find in the sweep lands here instead of
// silently reading garbage -- loud and obvious (messagebox + int3) rather than a mystery crash.
void __cdecl CSO_GameModeTrap_Report(DWORD dwReturnAddr)
{
	DWORD dwRVA = dwReturnAddr - g_dwClientBase;
	DWORD dwIdaAddr = dwRVA + 0x1900000; // client.dll IDA imagebase

	char buf[1024];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE,
		"CSO_FakeGameModeObj: unexpected vtable slot called (not covered by the sweep)!\n\n"
		"Return address (VA):   0x%08X\n"
		"Return address (RVA):  0x%08X\n"
		"IDA address (imagebase 0x1900000): 0x%08X\n\n"
		"Send this address over -- it's the instruction right after the offending "
		"\"call dword ptr [reg+off]\".",
		dwReturnAddr, dwRVA, dwIdaAddr);

	printf("[CSO_GameModeTrap] %s\n", buf);
	PrintStackTrace();

	MessageBox(NULL, buf, "Error", MB_OK);
}

__declspec(naked) void Thunk_GameMode_Trap()
{
	__asm mov eax, [esp]
		__asm push eax
	__asm call CSO_GameModeTrap_Report
	__asm add esp, 4
	__asm int 3
}


// mov ecx, dword ptr [g_dwGameModeRealObj]  ; 8B 0D imm32
// mov eax, dword ptr [ecx]                  ; 8B 01
// jmp dword ptr [eax + slotOffset]          ; FF A0 imm32
#pragma pack(push, 1)
struct GameModeThunkStub
{
	BYTE  mov_ecx_op;    // 0x8B
	BYTE  mov_ecx_modrm; // 0x0D
	DWORD mov_ecx_addr;  // &g_dwGameModeRealObj
	BYTE  mov_eax_op;    // 0x8B
	BYTE  mov_eax_modrm; // 0x01
	BYTE  jmp_op;        // 0xFF
	BYTE  jmp_modrm;     // 0xA0
	DWORD jmp_offset;    // this slot's vtable offset
};
#pragma pack(pop)

static GameModeThunkStub* g_gameModeThunkStubs = NULL; // GAMEMODE_VTABLE_SLOTS entries, VirtualAlloc'd executable

// ---- Preset "classicUI" data (applied by CSO_ApplyClassicUIPreset) ----
static const int g_classicUIWhitelist[] = {
		// Classic HUD. Every GetGameMode() call site whose switch is exactly 11/18/21/25/38/39 is on, except:
		//  - the first group below (clock, scoreboard, notification, round counter): kept on the real mode
		//  - the Karma sites further down: they must see the real mode or the Karma gauge disappears
		//  - 0x0119FD3C / 0x011A311D / 0x011A4DFD: the HUD frame backgrounds are removed through 0x011A65A0 (texture loader)
		//    plus the byte patches in CSO_SetClassicUIPatches
		//0x00F20A7E, <-- clock icon
		//0x00F20B25, <-- clock timer
		//0x00F20E9B,
		//0x00F22F77,
		//0x00F348B4,
		//0x00F34D89,
		//0x00F23C89, <-- Scoreboard on top mid
		//0x011D239D, <-- In-game notification
		//0x011753B6, <-- stupid round counter

		0x0114430F, 0x011FD1B8, // MAP location
		0x00F216BD,
		0x00F48E9A, 0x00F48FF5, 0x00F490FE, 0x00F52D4B, 0x00F5DB3F, 0x00F5EB80,
		0x01008F89,
		0x010AE3B4, // zombie skill bar Y: left on the real mode = 30 (classic would be 3)
		0x010BCE6B,
		0x010C4D43, 0x01107080, 0x0111B4A0, 0x0111B776, 0x0111B873, 0x0111B8D9, 0x0111B9AF, 0x0111C275, 0x0111C4EF, 0x0111C56B, 0x0111C61C, 0x0111C81E, 0x0111C875, 0x0111C8F5, 0x0111C965, 0x0111C9BA, 0x0111CA32, 0x0111CB24, 0x0111CC05, 0x0111CCAD, 0x0111CD37, 0x0111CDED, 0x0111CE9C,
		0x0111E69A, 0x01123335, 0x01123421, 0x011249A4, 0x01124C00, 0x01125156, 0x011253F6, 0x01125720, 0x01125BCF, 0x01125DA5, 0x01125EA3, 0x01126189, 0x0112DD53, 0x0112E6B0, 0x011308D8, 0x0113AEA1, 0x0113AF6A, 0x0113AFE2, 0x01142B38, /*0x0114430F,*/
		0x011445AD, 0x011459E0, 0x01147000, 0x0115A7C1, 0x0116FB12, 0x0116FDD1, 0x011701D7, 0x01170BF6, 0x01176357, 0x011764BF, 0x01176565, 0x01177C09, 0x01178A1D, 0x0118F5B0,
		0x011A328A,
		0x011A65A0, 0x011A6849, 0x011A736B, 0x011A7813, // HUD background (texture loader) + hostage/damage/nightvision
		//0x011B621E, 0x011C702D, 0x011C70D4, /*0x011C755D,*/ /*0x011C766A,*/ /*0x011C7AE0,*/
		/*0x011F1385,*/ /*0x011F175F,*/ /*0x011F18B0,*/ /*0x011F1B9F,*/ /*0x011FD1B8,*/

		// Karma (gauge / notices): keep on the real mode
		//0x010B8816, // sub_2732B70 -> sub_2847F00 (gauge update)
		//0x010BFD56, // sub_2739A00 -> sub_2848B30 (panel update)
		//0x00F48DB4, //0x00EC5835, //0x00F45C15, //0x00F49236, //0x00F46675, //0x00F4644C, //0x00F45F60, //0x00F45DBB, //0x011E04B7,

		// HUD frame: handled by 0x011A65A0 + byte patches instead
		//0x0119FD3C,
		//0x011A311D,
		//0x011A4DFD,
};

static const int g_classicUIBlacklist[] = {
		0x01177C09, // sub_27E7870 Tab scoreboard: always the real mode
};

// The lists the GetGameMode() thunk consults. A preset points them at its own arrays; NULL / 0 = nothing is overridden.
static const int* g_pWhitelist = NULL;
static int g_nWhitelist = 0;
static const int* g_pBlacklist = NULL;
static int g_nBlacklist = 0;

BOOL IsReturnAddrWhitelisted(uintptr_t rva)
{
	for (int i = 0; i < g_nWhitelist; i++)
		if ((uintptr_t)g_pWhitelist[i] == rva)
			return TRUE;
	return FALSE;
}

BOOL IsReturnAddrBlacklisted(uintptr_t rva)
{
	for (int i = 0; i < g_nBlacklist; i++)
		if ((uintptr_t)g_pBlacklist[i] == rva)
			return TRUE;
	return FALSE;
}

static void CSO_SetClassicUIPatches(bool enable);

static volatile LONG g_classicUIState = -1; // -1 unknown, 0 default UI, 1 classic UI

// The HUD object that owns the background texture slots, remembered from the texture loader (sub_2AA64A0, RVA 0x011A64A0).
// Going normal -> classic does not rebuild the HUD, and the loader's classic early-return leaves the textures of the
// previous mode in place, so the backgrounds would stay. Zeroing the slots makes sub_280F780 skip those draws (each
// draw is guarded by "cmp [slot], 0"). slot offsets: 98h weapon_bg_bottom, 9Ch dollar_bg, 0A0h character_bg_bottom,
// 0A4h maptitle_bg (its unguarded draw is jumped over by the byte patches), 0ACh portrait, 0B0h character_bg_top, 0B4h weapon_bg_top.
static void* g_pHudObject = NULL;

static void CSO_ZeroHudTextureSlots()
{
	BYTE* hud = (BYTE*)g_pHudObject;
	if (!hud)
		return;

	static const DWORD slots[] = { 0x98, 0x9C, 0xA0, 0xA4, 0xAC, 0xB0, 0xB4 };
	for (DWORD off : slots)
		*(DWORD*)(hud + off) = 0;
}

typedef int(__thiscall* fnHudLoadTextures)(void*);
static fnHudLoadTextures g_pfnHudLoadTextures = NULL;

static int __fastcall Hook_HudLoadTextures(void* thisptr, int /*edx*/)
{
	g_pHudObject = thisptr;
	int result = g_pfnHudLoadTextures(thisptr);

	// the loader just ran with the (possibly now classic) mode; make sure no stale textures survive in classic
	if (g_classicUIState == 1)
		CSO_ZeroHudTextureSlots();

	return result;
}

static bool CSO_IsAllowedGameMode(int mode)
{
	for (int allowed : allowedGameModes)
		if (allowed == mode)
			return true;
	return false;
}

// Called with the real game mode on every GetGameMode() call: writes the byte patches when the mode becomes allowed
// and reverts them when it stops being allowed. Several threads can arrive at once; only the first one to flip the
// state does the work.
static void CSO_UpdateClassicUIForMode(int realMode)
{
	const LONG want = CSO_IsAllowedGameMode(realMode) ? 1 : 0;

	if (g_classicUIState != want && InterlockedExchange(&g_classicUIState, want) != want)
		CSO_SetClassicUIPatches(want == 1);
}

// GetGameMode() override (vtable +0x188). ecx is the fake "this" (unused); edx is whatever the
// caller loaded from the fake vtable pointer (unused). g_iGameModeOverride == -1 passes the real
// value straight through, so the proxy is a no-op until you actually flip it for testing.
// The classic UI is only active while the real mode is in allowedGameModes; for every other mode the real value
// is returned from every call site (default UI).
int __fastcall Thunk_GameMode_Override(void* /*ecxFakeThis*/, void* /*edxUnused*/)
{
	uintptr_t retaddr = (uintptr_t)_ReturnAddress();
	uintptr_t rva = retaddr - g_dwClientBase;

	typedef int(__thiscall* fnGetGameMode)(void*);
	void* real = (void*)g_dwGameModeRealObj;
	DWORD realVtable = *(DWORD*)real;
	fnGetGameMode fn = *(fnGetGameMode*)(realVtable + 0x188);
	int realMode = fn(real);

	// Diagnostic: log (once per call site) every GetGameMode() call made from the Karma panel code,
	// so the console shows whether that code runs at all and which mode it sees.
	if (rva >= 0x011DA600 && rva < 0x011E1200)
	{
		static std::vector<uintptr_t> s_karmaLogged;
		if (std::find(s_karmaLogged.begin(), s_karmaLogged.end(), rva) == s_karmaLogged.end())
		{
			s_karmaLogged.push_back(rva);
			printf("[Karma] GetGameMode() called from RVA 0x%08X (real mode %d)\n", (unsigned)rva, realMode);
		}
	}

	// -1 = transparent: no override and no byte patches (pass a mode that is never allowed so they get reverted)
	CSO_UpdateClassicUIForMode(g_iGameModeOverride == -1 ? -1 : realMode);

	if (g_iGameModeOverride == -1)
		return realMode;

	if (CSO_IsAllowedGameMode(realMode) && IsReturnAddrWhitelisted(rva) && !IsReturnAddrBlacklisted(rva))
		return g_iGameModeOverride;

	return realMode;
}

struct CSO_FakeGameModeObj
{
	DWORD* vtable;
};

static CSO_FakeGameModeObj g_fakeGameModeObj;
static DWORD g_fakeGameModeVtable[GAMEMODE_VTABLE_SLOTS];

static void CSO_InitClientGameModeProxy()
{
	g_gameModeThunkStubs = (GameModeThunkStub*)VirtualAlloc(NULL, GAMEMODE_VTABLE_SLOTS * sizeof(GameModeThunkStub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!g_gameModeThunkStubs)
	{
		MessageBox(NULL, "GameModeThunkStubs VirtualAlloc failed!!!", "Error", MB_OK);
		return;
	}

	for (int i = 0; i < GAMEMODE_VTABLE_SLOTS; i++)
	{
		DWORD slotOffset = (DWORD)i * 4;
		GameModeThunkStub& stub = g_gameModeThunkStubs[i];

		stub.mov_ecx_op = 0x8B;
		stub.mov_ecx_modrm = 0x0D;
		stub.mov_ecx_addr = (DWORD)&g_dwGameModeRealObj;
		stub.mov_eax_op = 0x8B;
		stub.mov_eax_modrm = 0x01;
		stub.jmp_op = 0xFF;
		stub.jmp_modrm = 0xA0;
		stub.jmp_offset = slotOffset;

		g_fakeGameModeVtable[i] = (DWORD)&stub;
	}

	// GetGameMode() is the only slot we actually override; every other slot is a generated
	// passthrough stub regardless of which call-site shape reaches it.
	g_fakeGameModeVtable[0x188 / 4] = (DWORD)Thunk_GameMode_Override;

	g_fakeGameModeObj.vtable = g_fakeGameModeVtable;
}

// Installed in place of dword_3A83BE8. Refreshes g_dwGameModeRealObj from the real accessor on
// every call (so it stays correct across map/session changes) and always returns our fake object.
DWORD __cdecl Hook_GameModeAccessor()
{
	typedef DWORD(__cdecl* fnAccessor)();
	fnAccessor real = (fnAccessor)g_dwGameModeRealAccessor;
	g_dwGameModeRealObj = real();
	return (DWORD)&g_fakeGameModeObj;
}

// ---- classicUI byte patches (client.dll) -------------------------------------------------------------------------
// Applied when the real game mode is in allowedGameModes and reverted when it is not (see CSO_UpdateClassicUIForMode),
// so every other mode keeps the default UI. Each entry is verified against its expected bytes before it is written.
//
//  sub_280F780 (main HUD frame paint, called from sub_280C200), second GetGameMode() switch (ret 0x011A328A, cmp eax,3Ch):
//    cases 0,3,6,7,15-17,26,28,31,42,45,48,50,52,55 -> mov [ebp+var_D], 1   (Hex-Rays: v95 = 1), everything else -> 0.
//    The "1" becomes "0" so v95 is always 0; it only feeds the (v95 | vtable+690h(8)) check guarding the [this+9Ch]
//    hud_dollar_bg draw.
//
//  With 0x00F12B40 whitelisted, sub_2812A40 (HUD texture loader) takes the classic early-return and never fills the
//  texture slots / creates the portrait effect panel. sub_280F780 null-checks most slots but NOT these two:
//    [this+0A4h] hud_maptitle_bg      -> mov edx,[ecx] at 0x0280FFCC (read of 0x00000000)
//    [this+0A8h] CPortraitEffectPanel -> mov eax,[ecx] at 0x0280FC5E
//  so both unguarded uses are jumped over, and the portrait block (portrait + "Lv.") is skipped as well.
//
//  Karma gauge: KarmaPanel / DualKarmaPanel are placed with SetBounds(52, screenH - 226, 600, 118) (the gauge circle is a
//  62x62 child at y+56). With the portrait gone the gauge floats alone and collides with the chat, so it is moved:
//  KARMA_PANEL_Y_OFFSET is added to screenH (-226 = original, less negative = lower), KARMA_PANEL_X replaces the x.
#define KARMA_PANEL_Y_OFFSET (-186) // 40 px lower than the original
#define KARMA_PANEL_X 18            // original is 52 (0x34); must stay in 0..127 (it is a push imm8)

struct ClassicUIPatch
{
	DWORD rva;
	DWORD len;
	BYTE  original[6];
	BYTE  patched[6];
};

static void CSO_SetClassicUIPatches(bool enable)
{
	const BYTE yo0 = (BYTE)(KARMA_PANEL_Y_OFFSET & 0xFF), yo1 = (BYTE)((KARMA_PANEL_Y_OFFSET >> 8) & 0xFF);
	const BYTE yo2 = (BYTE)((KARMA_PANEL_Y_OFFSET >> 16) & 0xFF), yo3 = (BYTE)((KARMA_PANEL_Y_OFFSET >> 24) & 0xFF);

	const ClassicUIPatch patches[] = {
		// v95 flag
		{ 0x011A329D, 4, { 0xC6, 0x45, 0xEF, 0x01 },                         { 0xC6, 0x45, 0xEF, 0x00 } },                       // mov [ebp+var_11],1 -> 0
		// null guards / portrait block
		{ 0x011A3936, 2, { 0x74, 0x5C },                                     { 0xEB, 0x5C } },                                   // jz -> jmp: skip hud_maptitle_bg draw
		{ 0x011A35EA, 6, { 0x8B, 0x8B, 0xA8, 0x00, 0x00, 0x00 },             { 0xEB, 0x20, 0x90, 0x90, 0x90, 0x90 } },           // jmp over the portrait effect panel calls
		{ 0x011A361A, 6, { 0x0F, 0x84, 0xE9, 0x00, 0x00, 0x00 },             { 0xE9, 0xEA, 0x00, 0x00, 0x00, 0x90 } },           // jmp over the portrait block
		// scoreboard background (sub_2A77AA0): "test al, 9 ; jz" decides SBOriginalBG vs SBTeamDeathBG/...; never take the jz so SBOriginalBG is always used
		{ 0x01177AFC, 2, { 0x74, 0x12 },                                     { 0x90, 0x90 } },                                   // jz -> nop nop
		// Karma panel y: add eax, 0FFFFFF1Eh
		{ 0x011DB0DD, 5, { 0x05, 0x1E, 0xFF, 0xFF, 0xFF },                   { 0x05, yo0, yo1, yo2, yo3 } },                     // sub_2ADA670
		{ 0x011DBC64, 5, { 0x05, 0x1E, 0xFF, 0xFF, 0xFF },                   { 0x05, yo0, yo1, yo2, yo3 } },                     // sub_2ADB6F0
		{ 0x011DF4C0, 5, { 0x05, 0x1E, 0xFF, 0xFF, 0xFF },                   { 0x05, yo0, yo1, yo2, yo3 } },                     // sub_2ADF4B0
		{ 0x011DF520, 5, { 0x05, 0x1E, 0xFF, 0xFF, 0xFF },                   { 0x05, yo0, yo1, yo2, yo3 } },                     // sub_2ADF510
		// Karma panel x: push 34h
		{ 0x011DB0E3, 2, { 0x6A, 0x34 },                                     { 0x6A, (BYTE)KARMA_PANEL_X } },
		{ 0x011DBC6A, 2, { 0x6A, 0x34 },                                     { 0x6A, (BYTE)KARMA_PANEL_X } },
		{ 0x011DF4C8, 2, { 0x6A, 0x34 },                                     { 0x6A, (BYTE)KARMA_PANEL_X } },
		{ 0x011DF528, 2, { 0x6A, 0x34 },                                     { 0x6A, (BYTE)KARMA_PANEL_X } },
	};

	int changed = 0, skipped = 0;
	for (const ClassicUIPatch& p : patches)
	{
		BYTE* at = (BYTE*)(g_dwClientBase + p.rva);
		const BYTE* from = enable ? p.original : p.patched;
		const BYTE* to = enable ? p.patched : p.original;

		if (memcmp(at, to, p.len) == 0)
			continue; // already in the wanted state

		if (memcmp(at, from, p.len) != 0)
		{
			printf("[ClassicUI] unexpected bytes at RVA 0x%08X, skipped\n", p.rva);
			skipped++;
			continue;
		}

		WriteMemory(at, (BYTE*)to, p.len);
		changed++;
	}

	printf("[ClassicUI] %s: %d patch(es) written, %d skipped\n", enable ? "enabled" : "disabled", changed, skipped);

	// normal -> classic without a HUD rebuild: drop the textures the previous mode loaded (see g_pHudObject)
	if (enable)
		CSO_ZeroHudTextureSlots();
}

// Preset "classicUI": classic HUD with the Karma gauge kept, the new scoreboard / skill bar / location text.
// Applies only while the real game mode is in allowedGameModes; every other mode keeps the default UI.
//   1. g_iGameModeOverride = 21         the classic mode id the whitelisted GetGameMode() call sites see
//   2. g_classicUIWhitelist             which call sites get it (88 exact-switch sites minus the first commented group,
//                                       0x00E290BE off = skill bar stays at Y 30, 0x00F2202E off = chat at the new position,
//                                       0x00F31FF0 etc. off, Karma sites untouched, 0x00EB5D6F + 0x00F64D5E on = map location)
//   3. g_classicUIBlacklist             call sites that always see the real mode (0x00EE79D9)
//   4. CSO_SetClassicUIPatches          the byte patches (v95 flag, null guards + portrait block, Karma panel position)
// The mode gate lives in Thunk_GameMode_Override / CSO_UpdateClassicUIForMode: the thunk looks at the real mode on every
// call, only overrides it for allowed modes, and writes / reverts the byte patches when the allowed state changes.
// The HUD-rebuilding parts (texture loader etc.) take effect after a map change/reconnect.
void CSO_PatchClientGameModeProxy();

void CSO_ApplyClassicUIPreset()
{
	// 1-3: override value + the call-site lists, set BEFORE the proxy goes live
	g_iGameModeOverride = 21;
	g_pWhitelist = g_classicUIWhitelist;
	g_nWhitelist = (int)(sizeof(g_classicUIWhitelist) / sizeof(g_classicUIWhitelist[0]));
	g_pBlacklist = g_classicUIBlacklist;
	g_nBlacklist = (int)(sizeof(g_classicUIBlacklist) / sizeof(g_classicUIBlacklist[0]));

	// the GetGameMode() proxy itself (waits for the accessor slot, hooks it); the byte patches (4) are written by the
	// proxy as soon as it sees an allowed mode
	CSO_PatchClientGameModeProxy();

	// remember the HUD object from the texture loader (sub_2812A40); only hook it if the prologue is what we expect
	BYTE* loader = (BYTE*)(g_dwClientBase + 0x011A64A0);
	const BYTE prologue[] = { 0x55, 0x8B, 0xEC, 0x6A, 0xFF };
	if (memcmp(loader, prologue, sizeof(prologue)) == 0)
		InlineHook(loader, Hook_HudLoadTextures, (void*&)g_pfnHudLoadTextures);
	else
		printf("[ClassicUI] texture loader prologue mismatch, not hooked (backgrounds may stay when switching to classic)\n");
}

void CSO_PatchClientGameModeProxy()
{
	DWORD pSlot = g_dwClientBase + CLIENT_GAMEMODE_ACCESSOR_RVA;

	// The engine resolves this slot itself sometime after client.dll loads; wait for it rather
	// than assuming it's already populated the instant GetModuleHandle succeeds.
	for (int i = 0; i < 60 && !(*(DWORD*)pSlot); i++)
		Sleep(500);

	g_dwGameModeRealAccessor = *(DWORD*)pSlot;
	if (!g_dwGameModeRealAccessor)
	{
		MessageBox(NULL, "GameModeAccessor slot never resolved!!!", "Error", MB_OK);
		return;
	}

	CSO_InitClientGameModeProxy();

	DWORD newAccessor = (DWORD)Hook_GameModeAccessor;
	WriteMemory((void*)pSlot, (BYTE*)&newAccessor, sizeof(newAccessor));
}
