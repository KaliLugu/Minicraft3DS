#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../Render.h"

Entity newEntityDragonFire(Entity *parent, uByte type, int x, int y, float xa, float ya) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_DRAGONPROJECTILE;
    e.level = parent->level;
    e.dragonFire.age = 0;
    e.dragonFire.type = type;
    e.dragonFire.parent = parent;
    e.dragonFire.xa = xa;
    e.dragonFire.ya = ya;
    e.dragonFire.xx = x;
    e.dragonFire.yy = y;
    e.x = (int)x;
    e.y = (int)y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = true;

    return e;
}

void tickEntityDragonFire(Entity *e, PlayerData *nearestPlayer) {
    e->dragonFire.age++;
    if (e->dragonFire.age >= 30) {
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    e->dragonFire.xx += e->dragonFire.xa;
    e->dragonFire.yy += e->dragonFire.ya;
    e->x = (int)e->dragonFire.xx;
    e->y = (int)e->dragonFire.yy;

    if (nearestPlayer != NULL && intersects(nearestPlayer->entity, e->x + e->dragonFire.xa - e->xr, e->y + e->dragonFire.ya - e->yr, e->x + e->dragonFire.xa + e->xr, e->y + e->dragonFire.ya + e->yr)) {
        EntityVsEntity(e, &(nearestPlayer->entity));
        removeEntityFromList(e, e->level, &eManager);
    }
}

void renderEntityDragonFire(Entity *e, sInt x, sInt y) {
    if (e->dragonFire.type == 0) {
        renderTile8Rotated(x, y, 0, 40, 0, e->dragonFire.age * 0.349);
    } else {
        renderTile8(x, y, 1, 40 + (e->dragonFire.age / 10), 0);
    }
}
