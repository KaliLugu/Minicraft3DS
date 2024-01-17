#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../Render.h"

Entity newEntityPassive(int type, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_PASSIVE;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.passive.mtype = type;
    e.passive.health = 20;
    e.passive.dir = 0;
    e.passive.xa = 0;
    e.passive.ya = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;

    return e;
}

void tickEntityPassive(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0)
        e->hurtTime--;
    if (e->passive.randWalkTime == 0 && nearestPlayer != NULL) {
        int xd = nearestPlayer->entity.x - e->x;
        int yd = nearestPlayer->entity.y - e->y;
        // flee from player
        if (xd * xd + yd * yd < 40 * 40) {
            e->passive.xa = 0;
            e->passive.ya = 0;
            if (xd < 0)
                e->passive.xa = +1;
            if (xd > 0)
                e->passive.xa = -1;
            if (yd < 0)
                e->passive.ya = +1;
            if (yd > 0)
                e->passive.ya = -1;
        }
    }

    if (e->passive.xa < 0)
        e->passive.dir = 2;
    else if (e->passive.xa > 0)
        e->passive.dir = 3;
    if (e->passive.ya < 0)
        e->passive.dir = 1;
    else if (e->passive.ya > 0)
        e->passive.dir = 0;

    if (e->passive.xa != 0 || e->passive.ya != 0)
        e->passive.walkDist++;

    int pspeed = syncTickCount & 1;
    if (!moveMob(e, e->passive.xa * pspeed, e->passive.ya * pspeed) || (syncRand() % 100) == 0) {
        e->passive.randWalkTime = 60;
        e->passive.xa = ((syncRand() % 3) - 1) * (syncRand() % 2);
        e->passive.ya = ((syncRand() % 3) - 1) * (syncRand() % 2);
    }
    if (e->passive.randWalkTime > 0)
        e->passive.randWalkTime--;
}

void renderEntityPassive(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    switch (e->passive.dir) {
    case 0: // down
        renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 0, 6, ((e->passive.walkDist >> 4) & 1) == 0 ? 0 : 1);
        break;
    case 1: // up
        renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 1, 6, ((e->passive.walkDist >> 4) & 1) == 0 ? 0 : 1);
        break;
    case 2: // left
        renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 2 + ((e->passive.walkDist >> 4) & 1), 6, 1);
        break;
    case 3: // right
        renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 2 + ((e->passive.walkDist >> 4) & 1), 6, 0);
        break;
    }
}
