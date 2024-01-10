#pragma once

#include "../engine/graphics.h"
#include <3ds.h>

extern void renderTile8Rotated(int xp, int yp, int xTile, int yTile, u8 bits, float angle);
inline void renderTile8(int xp, int yp, int xTile, int yTile, u8 bits) {
    renderTile8Rotated(xp, yp, xTile, yTile, bits, 0);
}

extern void renderTile16(int xp, int yp, int xTile, int yTile, u8 bits);
extern void renderPlayerTile16(int xp, int yp, int xTile, int yTile, u8 bits, float scale);
extern void renderTile16Blend(int xp, int yp, int xTile, int yTile, u8 bits, Color color);

extern void renderTile32(int xp, int yp, int xTile, int yTile, u8 bits);
extern void renderTile64(int xp, int yp, int xTile, int yTile, u8 bits);

extern void renderTile(int i, int d, u8 level, int x, int y);
extern void renderConnectedTile4(int x, int y, u32 xTile, u32 yTile);
extern void renderConnectedTile8(int x, int y, u32 xTile, u32 yTile);
extern void renderBackground(s8 level, int xScroll, int yScroll);
