#include "EntityData.h"
#include "../Globals.h"
#include "../engine/dtypes.h"

struct _entityType {
    bool persist;
    // TODO: Move more "callbacks" to this "dynamic functions"
    // Note: These need to always be set (explicitly set to NULL if unused) or the game will crash
    // TODO: onDeathFunction
    // TODO: saveFunction
    // TODO: loadFunction
    void (*tick)(Entity *e, PlayerData *nearestPlayer);
    void (*render)(Entity *e, sInt x, sInt y);
};
static struct _entityType entityTypes[MAX_ENTITY_ID + 1] = {0};

static void _entityRegister(int id, bool persist, void (*tick)(Entity *e, PlayerData *nearestPlayer), void (*render)(Entity *e, sInt x, sInt y)) {
    entityTypes[id].persist = persist;
    entityTypes[id].tick = tick;
    entityTypes[id].render = render;
}

extern void tickEntityItem(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityZombie(Entity *e, PlayerData *nearestPlayer);
extern void tickEntitySlime(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityAirWizard(Entity *e, PlayerData *nearestPlayer);
extern void tickEntitySpark(Entity *e, PlayerData *nearestPlayer);
extern void tickParticleText(Entity *e, PlayerData *nearestPlayer);
extern void tickParticleSmash(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityPassive(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityArrow(Entity *e, PlayerData *nearestPlayer);
extern void tickEntitySkeleton(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityKnight(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityGlowworm(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityDragon(Entity *e, PlayerData *nearestPlayer);
extern void tickEntityDragonFire(Entity *e, PlayerData *nearestPlayer);

extern void renderEntityItem(Entity *e, sInt x, sInt y);
extern void renderEntityFurniture(Entity *e, sInt x, sInt y);
extern void renderEntityZombie(Entity *e, sInt x, sInt y);
extern void renderEntitySlime(Entity *e, sInt x, sInt y);
extern void renderEntityAirWizard(Entity *e, sInt x, sInt y);
extern void renderEntitySpark(Entity *e, sInt x, sInt y);
extern void renderParticleText(Entity *e, sInt x, sInt y);
extern void renderParticleSmash(Entity *e, sInt x, sInt y);
extern void renderEntityPassive(Entity *e, sInt x, sInt y);
extern void renderEntityArrow(Entity *e, sInt x, sInt y);
extern void renderEntitySkeleton(Entity *e, sInt x, sInt y);
extern void renderEntityKnight(Entity *e, sInt x, sInt y);
extern void renderEntityGlowworm(Entity *e, sInt x, sInt y);
extern void renderEntityDragon(Entity *e, sInt x, sInt y);
extern void renderEntityDragonFire(Entity *e, sInt x, sInt y);
extern void renderEntityMagicPillar(Entity *e, sInt x, sInt y);
extern void renderEntityNPC(Entity *e, sInt x, sInt y);

void entityDataInit() {
    _entityRegister(ENTITY_NULL, false, NULL, NULL);
    _entityRegister(ENTITY_ITEM, true, &tickEntityItem, &renderEntityItem);
    _entityRegister(ENTITY_FURNITURE, true, NULL, &renderEntityFurniture);
    _entityRegister(ENTITY_ZOMBIE, true, &tickEntityZombie, &renderEntityZombie);
    _entityRegister(ENTITY_SLIME, true, &tickEntitySlime, &renderEntitySlime);
    _entityRegister(ENTITY_AIRWIZARD, true, &tickEntityAirWizard, &renderEntityAirWizard);
    _entityRegister(ENTITY_SPARK, false, &tickEntitySpark, &renderEntitySpark);
    _entityRegister(ENTITY_TEXTPARTICLE, false, &tickParticleText, &renderParticleText);
    _entityRegister(ENTITY_SMASHPARTICLE, false, &tickParticleSmash, &renderParticleSmash);
    _entityRegister(ENTITY_PLAYER, false, NULL, NULL); // handled sepparately
    _entityRegister(ENTITY_PASSIVE, true, &tickEntityPassive, &renderEntityPassive);
    _entityRegister(ENTITY_ARROW, false, &tickEntityArrow, &renderEntityArrow);
    _entityRegister(ENTITY_SKELETON, true, &tickEntitySkeleton, &renderEntitySkeleton);
    _entityRegister(ENTITY_KNIGHT, true, &tickEntityKnight, &renderEntityKnight);
    _entityRegister(ENTITY_GLOWWORM, true, &tickEntityGlowworm, &renderEntityGlowworm);
    _entityRegister(ENTITY_DRAGON, true, &tickEntityDragon, &renderEntityDragon);
    _entityRegister(ENTITY_DRAGONPROJECTILE, false, &tickEntityDragonFire, &renderEntityDragonFire);
    _entityRegister(ENTITY_MAGIC_PILLAR, false, NULL, &renderEntityMagicPillar);
    _entityRegister(ENTITY_NPC, false, NULL, &renderEntityNPC);
}

void tickEntity(Entity *e) {
    if (entityTypes[e->type].tick != NULL) {
        PlayerData *nearestPlayer = getNearestPlayer(e->level, e->x, e->y);
        (*(entityTypes[e->type].tick))(e, nearestPlayer);
    }
}

void renderEntity(Entity *e, sInt x, sInt y) {
    if (entityTypes[e->type].render != NULL) {
        (*(entityTypes[e->type].render))(e, x, y);
    }
}

bool persistEntity(Entity *e) {
    return entityTypes[e->type].persist;
}
