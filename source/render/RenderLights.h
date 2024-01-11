#pragma once

#include "../Globals.h"
#include "../Render.h"

extern MTexture playerLightBake;
extern MTexture lanternLightBake;
extern MTexture glowwormLightBake;
extern MTexture glowwormBigLightBake;

extern void bakeLights();
extern void freeLightBakes();

extern void renderLightsToStencil(PlayerData *pd, bool force, bool invert, bool rplayer);
extern void renderLight(int x, int y, MTexture texture);
extern void resetStencil();