#pragma once
#include "Crafting.h"
#include "engine/engine.h"
#include <stdlib.h>

// Entity types
#define ENTITY_NULL 0
#define ENTITY_ITEM 1
#define ENTITY_FURNITURE 2
#define ENTITY_ZOMBIE 3
#define ENTITY_SLIME 4
#define ENTITY_AIRWIZARD 5
#define ENTITY_SPARK 6
#define ENTITY_TEXTPARTICLE 7
#define ENTITY_SMASHPARTICLE 8
#define ENTITY_PLAYER 9
#define ENTITY_PASSIVE 10
#define ENTITY_ARROW 11
#define ENTITY_SKELETON 12
#define ENTITY_KNIGHT 13
#define ENTITY_GLOWWORM 14

#define ENTITY_DRAGON 15
#define ENTITY_DRAGONPROJECTILE 16
#define ENTITY_MAGIC_PILLAR 17

#define ENTITY_NPC 18

typedef struct Entity Entity;

typedef struct _plrd PlayerData; // in order to not include Player.h and cause all sorts of problems

typedef struct
{
    sByte ax;
    sByte ay;
    uByte dir;
    sByte health;
    sByte stamina;
    sByte staminaRecharge;
    sByte staminaRechargeDelay;
    sByte attackTimer;
    uByte spawnTrigger;
    bool isDead;
    bool hasWon;
    bool hasWonSaved;
    sByte endTimer;
    sShort walkDist;
    bool isCarrying;
    bool isSwimming;
    int swimTimer;

    PlayerData *data;
} Player;

typedef struct
{
    float xa;
    float ya;
    float za;
    float xx;
    float yy;
    float zz;
    sShort age;
    Item item;
} EntityItem;

typedef struct
{
    sShort itemID;
    bool active;
    sByte r;        // light radius for lantern.
    Inventory *inv; // Points to chest inventory.
} EntityFurniture;

typedef struct
{
    uByte mtype;
    sByte xa;
    sByte ya;
    sShort health;
    sByte dir;
    sByte randWalkTime;
    sByte walkDist;
} PassiveMob;

typedef struct
{
    sByte xa;
    sByte ya;
    sShort health;
    sByte dir;
    sByte lvl;
    sByte randWalkTime;
    sByte walkDist;
    sByte randAttackTime;
    Color color;
} HostileMob;

typedef struct
{
    sByte xa;
    sByte ya;
    sShort health;
    sByte randWalkTime;
    sByte walkDist;
    sByte dir;
    int attackDelay;
    int attackTime;
    int attackType;
    sByte spriteAdjust;
} AirWizard;

typedef struct
{
    Entity *parent;
    sShort age;
    float xa;
    float ya;
    float xx;
    float yy;
} Spark;

typedef struct
{
    Entity *parent;
    sShort age;
    sShort itemID;
    sByte xa;
    sByte ya;
} Arrow;

typedef struct
{
    sByte xa;
    sByte ya;
    sShort health;
    sByte randWalkTime;
    sByte walkDist;
    sByte dir;
    int attackDelay;
    int attackTime;
    int attackType;
    int animTimer;
} Dragon;

typedef struct
{
    Entity *parent;
    uByte type;
    sShort age;
    float xa;
    float ya;
    float xx;
    float yy;
} DragonFire;

typedef struct
{
    sByte xa;
    sByte ya;
    sByte randWalkTime;
    sByte waitTime;
} Glowworm;

typedef struct
{
    uByte type;
} NPC;

typedef struct
{
    float xa;
    float ya;
    float za;
    float xx;
    float yy;
    float zz;
    sShort age;
    char *text;
    Color color;
} TextParticleEntity;

typedef struct
{
    sShort age;
} SmashParticleEntity;

struct Entity {
    sShort x;
    sShort y;
    sShort type;

    sByte xKnockback, yKnockback;
    uByte xr, yr;
    uByte level;
    sByte hurtTime;
    sShort slotNum; // Read-only. Do not mess with this.
    bool canPass;
    bool canSwim;
    // TODO: unify stuff like health, dir, ... in a Mob struct
    union {
        Player p;
        EntityItem entityItem;
        EntityFurniture entityFurniture;
        PassiveMob passive;
        HostileMob hostile;
        AirWizard wizard;
        Spark spark;
        Arrow arrow;
        Glowworm glowworm;
        Dragon dragon;
        DragonFire dragonFire;
        NPC npc;
        TextParticleEntity textParticle;
        SmashParticleEntity smashParticle;
    };

    // TODO: Move more "callbacks" to this "dynamic functions"
    // Note: These need to always be set (explicitly set to NULL if unused) or the game will crash
    void (*tickFunction)(Entity *e, PlayerData *pd);
    // TODO: onDeathFunction
    // TODO: renderFunction
    // TODO: saveFunction
    // TODO: loadFunction
};

typedef struct
{
    Entity entities[6][1000];
    sShort lastSlot[6];
    Inventory invs[300];
    sShort nextInv;
} EntityManager;

extern EntityManager eManager;
extern Entity nullEntity;

extern Entity newEntityItem(Item item, int x, int y, uByte level);
extern Entity newEntityFurniture(int itemID, Inventory *invPtr, int x, int y, uByte level);
extern Entity newEntityPassive(int type, int x, int y, uByte level);
extern Entity newEntityZombie(int lvl, int x, int y, uByte level);
extern Entity newEntitySkeleton(int lvl, int x, int y, uByte level);
extern Entity newEntityKnight(int lvl, int x, int y, uByte level);
extern Entity newEntitySlime(int lvl, int x, int y, uByte level);
extern Entity newEntityAirWizard(int x, int y, uByte level);
extern Entity newEntitySpark(Entity *parent, float xa, float ya);
extern Entity newEntityDragon(int x, int y, uByte level);
extern Entity newEntityDragonFire(Entity *parent, uByte type, int x, int y, float xa, float ya);
extern Entity newEntityMagicPillar(int x, int y, uByte level);
extern Entity newEntityArrow(Entity *parent, int itemID, sByte xa, sByte ya, uByte level);
extern Entity newEntityGlowworm(int x, int y, uByte level);
extern Entity newEntityNPC(int type, int x, int y, uByte level);

extern Entity newParticleText(char *str, Color color, int xa, int ya, uByte level);
extern Entity newParticleSmash(int xa, int ya, uByte level);

extern void addEntityToList(Entity e, EntityManager *em);
extern void removeEntityFromList(Entity *e, uByte level, EntityManager *em);
