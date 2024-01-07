#pragma once

#include <3ds.h>

extern void renderTile(int i, int d, u8 level, int x, int y);
extern void renderConnectedTile4(int x, int y, u32 xTile, u32 yTile);
extern void renderConnectedTile8(int x, int y, u32 xTile, u32 yTile);
extern void renderBackground(s8 level, int xScroll, int yScroll);
