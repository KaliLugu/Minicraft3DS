#pragma once

#include "../Entity.h"

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

#define MAX_ENTITY_ID 18

extern void entityDataInit();
extern void tickEntity(Entity *e);
extern void renderEntity(Entity *e, sInt x, sInt y);
extern bool persistEntity(Entity *e);
