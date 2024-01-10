#pragma once

#include "render/RenderLights.h"
#include "render/RenderText.h"
#include "render/RenderTiles.h"

#include "Globals.h"
#include "MapGen.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern int offsetX, offsetY;

extern void renderTitle(int x, int y);
extern void renderFrame(int x1, int y1, int x2, int y2, Color bgColor);

extern void renderWeather(uByte level, int xScroll, int yScroll);
extern void renderDayNight(PlayerData *pd);
extern void renderButtonIcon(sInt icon, int x, int y);

extern void renderGui(PlayerData *pd);
extern void renderZoomedMap(PlayerData *pd);
extern void renderPlayer(PlayerData *pd, float scale);

extern void renderFurniture(int itemID, int x, int y);
extern void renderEntity(Entity *e, int x, int y);
extern void renderEntities(uByte level, int x, int y, EntityManager *em);

extern void renderRecipes(RecipeManager *r, int xo, int yo, int x1, int y1, int selected);
extern void renderItemList(Inventory *inv, int xo, int yo, int x1, int y1, int selected);
extern void renderItemWithText(Item *item, int x, int y);
extern void renderItemStuffWithText(int itemID, int itemCL, bool onlyOne, int x, int y);
extern void renderItemWithTextCentered(Item *item, int width, int y);
extern void renderItemIcon(int itemID, int countLevel, int x, int y);
