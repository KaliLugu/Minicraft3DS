#pragma once

#include "../Globals.h"
#include "../Render.h"

extern sf2d_texture *playerLightBake;
extern sf2d_texture *lanternLightBake;
extern sf2d_texture *glowwormLightBake;
extern sf2d_texture *glowwormBigLightBake;

extern void bakeLights();
extern void freeLightBakes();

extern void renderLightsToStencil(PlayerData *pd, bool force, bool invert, bool rplayer);
extern void renderLight(int x, int y, sf2d_texture *texture);
extern void resetStencil();