#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../Render.h"

// to mose, see comment in main.c
int itemGetLegacyId(ItemID id)
{
    switch (id.category)
    {
        case ITEM_CATEGORY_TOOL:
            if (id.id >= toolItemCount) return 0;
            return toolItems[id.id].legacy_id;

        case ITEM_CATEGORY_FOOD:
            if (id.id >= foodItemCount) return 0;
            return foodItems[id.id].legacy_id;

        case ITEM_CATEGORY_GENERIC:
            if (id.id >= genericItemCount) return 0;
            return genericItems[id.id].legacy_id;

        default:
            return 0;
    }
}

Entity newEntityArrow(Entity *parent, int itemID, sByte xa, sByte ya, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_ARROW;
    e.level = level;
    e.arrow.age = 0;
    e.arrow.parent = parent;
    e.arrow.itemID = itemID;
    e.arrow.xa = xa;
    e.arrow.ya = ya;
    e.x = parent->x;
    e.y = parent->y;
    e.xr = 2;
    e.yr = 2;
    e.canPass = false;
    e.canSwim = true;

    return e;
}

void tickEntityArrow(Entity *e, PlayerData *nearestPlayer) {
    e->arrow.age++;
    if (e->arrow.age >= 260 || !move(e, e->arrow.xa, e->arrow.ya)) {
        // only drop arrows shot by player
        if (e->arrow.parent->type == ENTITY_PLAYER)
            addItemsToWorld(newItem(e->arrow.itemID, 1), e->level, e->x + 4, e->y + 4, 1);
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
}

void renderEntityArrow(Entity *e, sInt x, sInt y) {
    if (e->arrow.age >= 200)
        if (e->arrow.age / 6 % 2 == 0)
            return;

    int abits = 0;
    int ayp = 21;
    if (e->arrow.xa < 0) {
        abits += 1;
    }
    if (e->arrow.ya < 0) {
        ayp += 1;
    }
    if (e->arrow.ya > 0) {
        ayp += 1;
        abits += 2;
    }

    // todo in future : compt the different type of arrow for more modularity and use for cycle 
    renderEntityShadowSmall(x + 2, y + 4);
    if (e->arrow.itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 27})) {
        renderTile8(x - 2, y - 2, 9, ayp, abits);
    } else if (e->arrow.itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 28})) {
        renderTile8(x - 2, y - 2, 10, ayp, abits);
    } else if (e->arrow.itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 29})) {
        renderTile8(x - 2, y - 2, 11, ayp, abits);
    } else if (e->arrow.itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 30})) {
        renderTile8(x - 2, y - 2, 12, ayp, abits);
    } else if (e->arrow.itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 31})) {
        renderTile8(x - 2, y - 2, 13, ayp, abits);
    }
}
