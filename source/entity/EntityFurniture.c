#include "../Entity.h"

#include "../Data.h"
#include "../Item.h"
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

void assignInventory(Entity *e) {
    if (eManager.nextInv > 300)
        return;
    e->entityFurniture.inv = &eManager.invs[eManager.nextInv];
    eManager.nextInv++;
}

Entity newEntityFurniture(int itemID, Inventory *invPtr, int x, int y, uByte level) {
    Entity e = {0}; // NOTE: always set to 0 to prevent uninitialized garbage data from causing issues (desyncs)
    e.type = ENTITY_FURNITURE;
    e.level = level;
    e.x = x;
    e.y = y;
    e.xr = 3;
    e.yr = 3;
    e.entityFurniture.itemID = itemID;
    e.canPass = false;
    if (itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 5})) { // Lantern
        e.entityFurniture.r = 8;
    } else if (itemID == itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 1})) { // TODO: This should not happen here, the new functions should not mess with any outside state
        if (invPtr == NULL)
            assignInventory(&e);
        else
            e.entityFurniture.inv = invPtr;
    }

    return e;
}

void renderEntityFurniture(Entity *e, sInt x, sInt y) {
    renderEntityShadow(x, y);
    renderFurniture(e->entityFurniture.itemID, x - 8, y - 8);
}
