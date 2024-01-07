#pragma once

#include "render/RenderLights.h"
#include "render/RenderText.h"
#include "render/RenderTiles.h"

#include "Globals.h"
#include "MapGen.h"
#include <3ds.h>
#include <ctype.h>
#include <sf2d.h>
#include <stdlib.h>
#include <string.h>

extern int offsetX, offsetY;
extern int playerScale;

extern void render(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits);
extern void renderb(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, u32 color);
extern void renderr(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, float rotate);
extern void renderc(s32 xp, s32 yp, u32 xTile, u32 yTile, int sizeX, int sizeY, u8 bits);
extern void render16(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits);
extern void render16c(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, float scaleX, float scaleY);
extern void render16b(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, u32 color);
extern void render32(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits);

extern void renderTitle(int x, int y);
extern void renderFrame(int x1, int y1, int x2, int y2, u32 bgColor);

extern void renderWeather(u8 level, int xScroll, int yScroll);
extern void renderDayNight(PlayerData *pd);
extern void renderButtonIcon(u32 icon, int x, int y, float scale);

extern void renderGui(PlayerData *pd);
extern void renderZoomedMap(PlayerData *pd);
extern void renderPlayer(PlayerData *pd);

extern void renderFurniture(int itemID, int x, int y);
extern void renderEntity(Entity *e, int x, int y);
extern void renderEntities(u8 level, int x, int y, EntityManager *em);

extern void renderRecipes(RecipeManager *r, int xo, int yo, int x1, int y1, int selected);
extern void renderItemList(Inventory *inv, int xo, int yo, int x1, int y1, int selected);
extern void renderItemWithText(Item *item, int x, int y);
extern void renderItemStuffWithText(int itemID, int itemCL, bool onlyOne, int x, int y);
extern void renderItemWithTextCentered(Item *item, int width, int y);
extern void renderItemIcon(int itemID, int countLevel, int x, int y);
extern void renderItemIcon2(int itemID, int countLevel, int x, int y, int z);
