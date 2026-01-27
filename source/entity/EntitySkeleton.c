#include "../data/items/ItemsData.h"

#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../Render.h"

Entity newEntitySkeleton(int lvl, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_SKELETON;
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
    e.hostile.randAttackTime = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;
    switch (lvl) {
    case 2:
        e.hostile.color = 0xC4C4C4FF;
        break;
    case 3:
        e.hostile.color = 0xA0A0A0FF;
        break;
    case 4:
        e.hostile.color = 0x7A7A7AFF;
        break;
    default:
        e.hostile.color = 0xFFFFFFFF;
        break;
    }

    return e;
}

void tickEntitySkeleton(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0)
        e->hurtTime--;

    if (e->hostile.xa < 0)
        e->hostile.dir = 2;
    else if (e->hostile.xa > 0)
        e->hostile.dir = 3;
    if (e->hostile.ya < 0)
        e->hostile.dir = 1;
    else if (e->hostile.ya > 0)
        e->hostile.dir = 0;

    // shooting arrows
    --(e->hostile.randAttackTime);
    if (e->hostile.randAttackTime <= 0) {
        e->hostile.randAttackTime = 80 - (e->hostile.lvl * 5);

        int aitemID = itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 27});
        if (e->hostile.lvl >= 2)
            aitemID = itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 28});

        // turn to player when attacking
        if (nearestPlayer != NULL) {
            int xd = nearestPlayer->entity.x - e->x;
            int yd = nearestPlayer->entity.y - e->y;
            if (xd * xd > yd * yd) {
                if (xd < 0)
                    e->hostile.dir = 2;
                if (xd > 0)
                    e->hostile.dir = 3;
            } else {
                if (yd < 0)
                    e->hostile.dir = 1;
                if (yd > 0)
                    e->hostile.dir = 0;
            }
        }

        switch (e->hostile.dir) {
        case 0:
            addEntityToList(newEntityArrow(e, aitemID, 0, 2, e->level), &eManager);
            break;
        case 1:
            addEntityToList(newEntityArrow(e, aitemID, 0, -2, e->level), &eManager);
            break;
        case 2:
            addEntityToList(newEntityArrow(e, aitemID, -2, 0, e->level), &eManager);
            break;
        case 3:
            addEntityToList(newEntityArrow(e, aitemID, 2, 0, e->level), &eManager);
            break;
        }
    }

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

void renderEntitySkeleton(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    switch (e->hostile.dir) {
    case 0: // down
        renderTile16Blend(x - 8, y - 8, 0, 5, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
        break;
    case 1: // up
        renderTile16Blend(x - 8, y - 8, 1, 5, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
        break;
    case 2: // left
        renderTile16Blend(x - 8, y - 8, 2 + ((e->hostile.walkDist >> 4) & 1), 5, 1, e->hostile.color);
        break;
    case 3: // right
        renderTile16Blend(x - 8, y - 8, 2 + ((e->hostile.walkDist >> 4) & 1), 5, 0, e->hostile.color);
        break;
    }
}
