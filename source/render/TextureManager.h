#pragma once
#include "../engine/engine.h"

extern Texture bottomBGFull;
extern Texture minimap[6];
extern Texture font[256 / 8 * 2];
extern Texture tiles8[(512 / 8) * (512 / 8)];
extern Texture tiles16[(512 / 16) * (512 / 16)];
extern Texture tiles32[(512 / 32) * (512 / 32)];
extern Texture tiles64[(512 / 64) * (512 / 64)];
extern Texture playerTiles[(736 / 16) * (160 / 16)];

extern Color dirtColor[5];
extern Color grassColor;
extern Color sandColor;
extern Color waterColor[2];
extern Color lavaColor[2];
extern Color rockColor[4];
extern Color woodColor;
extern Color ironColor;
extern Color goldColor;
extern Color gemColor;
extern Color dungeonColor[2];
extern Color myceliumColor;
extern Color mushroomColor;
extern Color snowColor;
extern Color iceColor;

extern void textureManagerReload();
