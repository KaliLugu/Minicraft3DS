#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../Render.h"

Entity newEntityZombie(int lvl, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_ZOMBIE;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.hostile.lvl = lvl;
    e.hostile.xa = 0;
    e.hostile.ya = 0;
    e.hostile.health = lvl * lvl * 10;
    e.hostile.dir = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;
    switch (lvl) {
    case 2:
        e.hostile.color = 0xCC8282FF;
        break;
    case 3:
        e.hostile.color = 0xEFEFEFFF;
        break;
    case 4:
        e.hostile.color = 0x6262AAFF;
        break;
    default:
        e.hostile.color = 0x95DB95FF;
        break;
    }

    return e;
}

void tickEntityZombie(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0)
        e->hurtTime--;
    if (e->hostile.randWalkTime == 0 && nearestPlayer != NULL) {
        int xd = nearestPlayer->entity.x - e->x;
        int yd = nearestPlayer->entity.y - e->y;
        int dist = 50 * 50;

        // charge player
        if (xd * xd + yd * yd < dist) {
            e->hostile.xa = 0;
            e->hostile.ya = 0;
            if (xd < 0)
                e->hostile.xa = -1;
            if (xd > 0)
                e->hostile.xa = +1;
            if (yd < 0)
                e->hostile.ya = -1;
            if (yd > 0)
                e->hostile.ya = +1;
        }
    }

    if (e->hostile.xa < 0)
        e->hostile.dir = 2;
    else if (e->hostile.xa > 0)
        e->hostile.dir = 3;
    if (e->hostile.ya < 0)
        e->hostile.dir = 1;
    else if (e->hostile.ya > 0)
        e->hostile.dir = 0;

    if (e->hostile.xa != 0 || e->hostile.ya != 0)
        e->hostile.walkDist++;

    int speed = syncTickCount & 1;
    if (!moveMob(e, e->hostile.xa * speed, e->hostile.ya * speed) || (syncRand() % 100) == 0) {
        e->hostile.randWalkTime = 60;
        e->hostile.xa = ((syncRand() % 3) - 1) * (syncRand() % 2);
        e->hostile.ya = ((syncRand() % 3) - 1) * (syncRand() % 2);
    }
    if (e->hostile.randWalkTime > 0)
        e->hostile.randWalkTime--;
}

void renderEntityZombie(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    switch (e->hostile.dir) {
    case 0: // down
        renderTile16Blend(x - 8, y - 8, 4, 7, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
        break;
    case 1: // up
        renderTile16Blend(x - 8, y - 8, 5, 7, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
        break;
    case 2: // left
        renderTile16Blend(x - 8, y - 8, 6 + ((e->hostile.walkDist >> 4) & 1), 7, 1, e->hostile.color);
        break;
    case 3: // right
        renderTile16Blend(x - 8, y - 8, 6 + ((e->hostile.walkDist >> 4) & 1), 7, 0, e->hostile.color);
        break;
    }
}
