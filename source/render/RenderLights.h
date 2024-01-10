#pragma once

#include "../Globals.h"
#include "../Render.h"

extern Texture playerLightBake;
extern Texture lanternLightBake;
extern Texture glowwormLightBake;
extern Texture glowwormBigLightBake;

extern void bakeLights();
extern void freeLightBakes();

extern void renderLightsToStencil(PlayerData *pd, bool force, bool invert, bool rplayer);
extern void renderLight(int x, int y, Texture *texture);
extern void resetStencil();