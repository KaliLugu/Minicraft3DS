#pragma once

#include "Data.h"
#include "Entity.h"
#include "Input.h"
#include "MapGen.h"
#include "Menu.h"
#include "Player.h"
#include "Quests.h"
#include <3ds.h>

#define CIRCLEPAD 0xF0000000
#define CSTICK 0x0F000000

#define MENU_PAUSED 100
#define MENU_INVENTORY 101
#define MENU_CRAFTING 102
#define MENU_CONTAINER 103
#define MENU_WIN 104
#define MENU_LOSE 105
#define MENU_DUNGEON 106
#define MENU_NPC 107
#define MENU_CHARACTER_CUSTOMIZE 108

#define NPC_GIRL 0
#define NPC_PRIEST 1
#define NPC_FARMER 2
#define NPC_LIBRARIAN 3
#define NPC_DWARF 4

// WARNING: Having this set to different values in different clients will break multiplayer!
// TODO: Change back before any release
#define TESTGODMODE true

extern u32 localUID;

extern int loadedtp;

extern u8 MODEL_3DS;

extern char versionText[34];

extern bool shouldRenderDebug;
extern bool shouldSpeedup;

extern Image imageIcons;
extern Image imagePlayerSprites;
extern Image imageFont;
extern Image imageBottombg;
extern Image imageMinimap[6];

extern char currentFileName[256];
extern u8 currentMenu;
extern char fpsstr[];
extern u8 initGame;
extern u8 initMPGame;
extern u8 initBGMap;
extern Item noItem;
extern int airWizardHealthDisplay;
extern s16 awX, awY;
extern bool quitGame;
extern s8 currentSelection;

typedef struct _worldData {
    u8 map[6][128 * 128];
    u8 data[6][128 * 128];

    u16 daytime;
    int day;
    u8 season;
    bool rain;

    u8 compassData[6][3];
} WorldData;

extern WorldData worldData;

extern u32 syncTickCount;

// TODO: cleanup the order
extern void addItemsToWorld(Item item, s8 level, int x, int y, int count);
extern bool intersects(Entity e, int x0, int y0, int x1, int y1);
extern int getEntities(Entity **result, s8 level, int x0, int y0, int x1, int y1);

// TODO: the move functions should probably be part of entity.c
extern bool move(Entity *e, int xa, int ya);
extern bool moveMob(Entity *e, int xa, int ya);
extern void hurtEntity(Entity *e, int damage, int dir, u32 hurtColor, Entity *damager);

extern void tickTile(s8 level, int x, int y);
extern bool tileIsSolid(int tile, Entity *e);

extern s8 itemTileInteract(int tile, PlayerData *pd, Item *item, s8 level, int x, int y, int px, int py, int dir);

extern void tickEntity(Entity *e);

extern void trySpawn(int count, int level);

extern int getTile(s8 level, int x, int y);
extern void setTile(int id, s8 level, int x, int y);
extern int getData(s8 level, int x, int y);
extern void setData(int id, s8 level, int x, int y);
extern Color getTileColor(int tile);

extern bool intersectsEntity(int x, int y, int r, Entity *e);

extern bool EntityBlocksEntity(Entity *e1, Entity *e2);
extern void EntityVsEntity(Entity *e1, Entity *e2);
extern bool ItemVsEntity(PlayerData *pd, Item *item, Entity *e, int dir);
extern void entityTileInteract(Entity *e, int tile, s8 level, int x, int y);

extern void openCraftingMenu(PlayerData *pd, RecipeManager *rm, char *title);
extern bool useEntity(PlayerData *pd, Entity *e);

extern bool isWater(s8 level, int xt, int yt);

extern void playerHurtTile(PlayerData *pd, int tile, s8 level, int xt, int yt, int damage, int dir);
extern void playerEntityInteract(PlayerData *pd, Entity *e);

extern bool dungeonActive();
extern void enterDungeon(PlayerData *pd);
extern void leaveDungeon(PlayerData *pd);

extern void setMinimapVisible(PlayerData *pd, int level, int x, int y, bool visible);
extern bool getMinimapVisible(PlayerData *pd, int level, int x, int y);
extern u32 getMinimapColor(PlayerData *pd, int level, int x, int y);
extern void initMinimapLevel(PlayerData *pd, int level);
extern void updateLevel1Map();
