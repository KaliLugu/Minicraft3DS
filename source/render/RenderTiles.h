#pragma once

#include "../engine/engine.h"

extern void renderTile8Rotated(int xp, int yp, int xTile, int yTile, int bits, float angle);
inline void renderTile8(int xp, int yp, int xTile, int yTile, int bits) {
    renderTile8Rotated(xp, yp, xTile, yTile, bits, 0);
}

extern void renderTile16(int xp, int yp, int xTile, int yTile, int bits);
extern void renderPlayerTile16(int xp, int yp, int xTile, int yTile, int bits, float scale);
extern void renderTile16Blend(int xp, int yp, int xTile, int yTile, int bits, Color color);

extern void renderTile32(int xp, int yp, int xTile, int yTile, int bits);
extern void renderTile64(int xp, int yp, int xTile, int yTile, int bits);

extern void renderTile(int i, int d, uByte level, int x, int y);
extern void renderConnectedTile4(int x, int y, int xTile, int yTile);
extern void renderConnectedTile8(int x, int y, int xTile, int yTile);
extern void renderBackground(uByte level, int xScroll, int yScroll);
