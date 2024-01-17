#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../Render.h"

Entity newEntitySlime(int lvl, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_SLIME;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.hostile.lvl = lvl;
    e.hostile.xa = 0;
    e.hostile.ya = 0;
    e.hostile.dir = 0;
    e.hostile.health = lvl * lvl * 5;
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

void tickEntitySlime(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0)
        e->hurtTime--;

    if (!moveMob(e, e->hostile.xa, e->hostile.ya) || (syncRand() % 10) == 0) {
        if (e->hostile.randWalkTime <= -10) {
            e->hostile.xa = ((syncRand() % 3) - 1);
            e->hostile.ya = ((syncRand() % 3) - 1);

            if (nearestPlayer != NULL) {
                int xd = nearestPlayer->entity.x - e->x;
                int yd = nearestPlayer->entity.y - e->y;
                if (xd * xd + yd * yd < 50 * 50) {
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

            if (e->hostile.xa != 0 || e->hostile.ya != 0)
                e->hostile.randWalkTime = 10;
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

    if (e->hostile.randWalkTime > -10)
        e->hostile.randWalkTime--;
    if (e->hostile.randWalkTime == 0) {
        e->hostile.xa = 0;
        e->hostile.ya = 0;
    }
}

void renderEntitySlime(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    renderTile16Blend(x - 8, y - 8 - (e->hostile.randWalkTime > 0 ? 4 : 0), 8 + (e->hostile.randWalkTime > 0 ? 1 : 0), 7, 0, e->hostile.color);
}
