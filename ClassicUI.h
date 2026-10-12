#pragma once

#include <windows.h>

// Mode id the whitelisted GetGameMode() call sites see (21 = classic CS HUD, -1 = pass the real value through).
extern int g_iGameModeOverride;

// Applies the whole "classicUI" preset to client.dll: GetGameMode() proxy, whitelist/blacklist, byte patches.
// Needs g_dwClientBase to be set (client.dll loaded). Blocks up to ~30 s waiting for the engine to fill the accessor slot.
void CSO_ApplyClassicUIPreset();
