#pragma once
#include "Data.h"

#include "Crafting.h"
#include "Entity.h"
#include "Inputs.h"
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

    uByte legs;
    uByte body;
    uByte arms;
    uByte head;
    uByte eyes;
    uByte accs;
} PlayerSprite;

typedef struct _plrefx {
    uByte level;
    sInt time;
} PlayerEffect;

typedef struct _plrd {
    // for identification in save data and sync game start
    sInt id;

    // input/multiplayer/synchronization
    Inputs inputs;
    Inputs nextInputs[MAX_INPUT_BUFFER];
    bool nextTurnReady[MAX_INPUT_BUFFER];

    //
    bool isSpawned;
    uByte minimapData[128 * 128];

    int score;
    QuestlineManager questManager;

    Entity entity;
    Inventory inventory;
    Item *activeItem;

    PlayerEffect effects[EFFECTS_MAX];

    PlayerSprite sprite;

    // menu data
    int ingameMenu;
    int ingameMenuSelection;
    int ingameMenuInvSel;
    int ingameMenuInvSelOther;
    bool ingameMenuAreYouSure;
    bool ingameMenuAreYouSureSave;
    sShort ingameMenuTimer;
    NPC_MenuData npcMenuData;

    RecipeManager currentRecipes;
    char *currentCraftTitle;
    Entity *curChestEntity;
    int curChestEntityR;

    bool mapShouldRender;
    int mapZoomLevel;
    int mapScrollX;
    int mapScrollY;
    char mapText[32];

    sInt touchLastX;
    sInt touchLastY;
    bool touchIsDraggingMap;
    bool touchIsChangingSize;
} PlayerData;

extern PlayerData players[MAX_PLAYERS];
extern int playerCount;
extern int playerLocalIndex;

extern void initPlayers();
extern void freePlayers();

extern void initPlayer(PlayerData *pd);
extern void freePlayer(PlayerData *pd);

extern PlayerData *getNearestPlayer(uByte level, int x, int y);
extern PlayerData *getLocalPlayer();

extern void tickPlayer(PlayerData *pd, bool inmenu);
extern void playerSetActiveItem(PlayerData *pd, Item *item);
extern bool playerUseEnergy(PlayerData *pd, int amount);
extern void playerHeal(PlayerData *pd, int amount);
extern void playerDamage(PlayerData *pd, int damage, int dir, MColor hurtColor, Entity *damager);
extern void playerSpawn(PlayerData *pd);

// effects
extern void playerEffectsUpdate(PlayerData *pd);
extern bool playerEffectActive(PlayerData *pd, int effect);
extern void playerEffectApply(PlayerData *pd, int effect, uByte level, sInt time);
extern void playerEffectRemove(PlayerData *pd, int effect);
extern uByte playerEffectGetLevel(PlayerData *pd, int effect);
extern sInt playerEffectGetTime(PlayerData *pd, int effect);
