#pragma once

#include "Crafting.h"
#include "Data.h"
#include "Entity.h"
#include "Input.h"
#include "QuestsData.h"

#define MAX_PLAYERS 8
#define MAX_INPUT_BUFFER 3

#define PLAYER_SPRITE_HEAD_COUNT 5
#define PLAYER_SPRITE_EYES_COUNT 5
#define PLAYER_SPRITE_BODY_COUNT 7
#define PLAYER_SPRITE_ARMS_COUNT 7
#define PLAYER_SPRITE_LEGS_COUNT 6
#define PLAYER_SPRITE_ACCS_COUNT 6

#define PLAYER_SPRITE_ACCS_FRONT             \
    {                                        \
        true, true, false, false, true, true \
    }

// TODO: Implement !GUI(with armor and effects, next to inventory?)!

typedef struct _plrsp {
    bool choosen;

    u8 legs;
    u8 body;
    u8 arms;
    u8 head;
    u8 eyes;
    u8 accs;
} PlayerSprite;

typedef struct _plrefx {
    u8 level;
    u32 time;
} PlayerEffect;

typedef struct _plrd {
    // for identification in save data and sync game start
    u32 id;
    bool idSet;
    bool ready;

    // input/multiplayer/synchronization
    Inputs inputs;
    Inputs nextInputs[MAX_INPUT_BUFFER];
    bool nextTurnReady[MAX_INPUT_BUFFER];

    //
    bool isSpawned;
    u8 minimapData[128 * 128];

    int score;
    QuestlineManager questManager;

    Entity entity;
    Inventory inventory;
    Item *activeItem;

    PlayerEffect effects[EFFECTS_MAX];

    PlayerSprite sprite;

    // menu data
    u8 ingameMenu;
    s8 ingameMenuSelection;
    s16 ingameMenuInvSel;
    s16 ingameMenuInvSelOther;
    bool ingameMenuAreYouSure;
    bool ingameMenuAreYouSureSave;
    s16 ingameMenuTimer;
    NPC_MenuData npcMenuData;

    RecipeManager currentRecipes;
    char *currentCraftTitle;
    Entity *curChestEntity;
    s8 curChestEntityR;

    bool mapShouldRender;
    u8 mapZoomLevel;
    s16 mapScrollX;
    s16 mapScrollY;
    char mapText[32];

    s16 touchLastX;
    s16 touchLastY;
    bool touchIsDraggingMap;
    bool touchIsChangingSize;
} PlayerData;

extern PlayerData players[MAX_PLAYERS];
extern int playerCount;
extern int playerLocalID;

extern void initPlayers();
extern void freePlayers();

extern void initPlayer(PlayerData *pd);
extern void freePlayer(PlayerData *pd);

extern PlayerData *getNearestPlayer(s8 level, s16 x, s16 y);
extern PlayerData *getLocalPlayer();

extern void tickPlayer(PlayerData *pd, bool inmenu);
extern void playerSetActiveItem(PlayerData *pd, Item *item);
extern bool playerUseEnergy(PlayerData *pd, int amount);
extern void playerHeal(PlayerData *pd, int amount);
extern void playerDamage(PlayerData *pd, int damage, int dir, u32 hurtColor, Entity *damager);
extern void playerSpawn(PlayerData *pd);

// effects
extern void playerEffectsUpdate(PlayerData *pd);
extern bool playerEffectActive(PlayerData *pd, int effect);
extern void playerEffectApply(PlayerData *pd, int effect, u8 level, u32 time);
extern void playerEffectRemove(PlayerData *pd, int effect);
extern u8 playerEffectGetLevel(PlayerData *pd, int effect);
extern u32 playerEffectGetTime(PlayerData *pd, int effect);
