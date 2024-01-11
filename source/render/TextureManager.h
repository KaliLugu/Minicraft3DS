#pragma once
#include "../engine/engine.h"

extern MTexture bottomBGFull;
extern MTexture minimap[6];
extern MTexture font[256 / 8 * 2];
extern MTexture tiles8[(512 / 8) * (512 / 8)];
extern MTexture tiles16[(512 / 16) * (512 / 16)];
extern MTexture tiles32[(512 / 32) * (512 / 32)];
extern MTexture tiles64[(512 / 64) * (512 / 64)];
extern MTexture playerTiles[(736 / 16) * (160 / 16)];

extern MColor dirtColor[5];
extern MColor grassColor;
extern MColor sandColor;
extern MColor waterColor[2];
extern MColor lavaColor[2];
extern MColor rockColor[4];
extern MColor woodColor;
extern MColor ironColor;
extern MColor goldColor;
extern MColor gemColor;
extern MColor dungeonColor[2];
extern MColor myceliumColor;
extern MColor mushroomColor;
extern MColor snowColor;
extern MColor iceColor;

extern void textureManagerReload();
