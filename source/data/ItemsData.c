#include "items/ItemsData.h"
#include "items/ItemsTypes.h"
#include <stdio.h>

static char *_itemNames[maxItemId + 1];
static int _itemIconX[maxItemId + 1];
static int _itemIconY[maxItemId + 1];
static bool _itemSingle[maxItemId + 1];

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

static void _itemRegister(int id, char *name, int iconX, int iconY, bool isSingle) {
    _itemNames[id] = name;
    _itemIconX[id] = iconX; // sur la largeur
    _itemIconY[id] = iconY; // sur la hauteur
    _itemSingle[id] = isSingle;
}

void itemsDataInit() {

    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 0}), "", 0, 0, true);

    // tools
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), "Shovel", 0, 18, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), "Hoe", 5, 18, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), "Sword", 10, 18, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), "Pickaxe", 15, 18, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), "Axe", 20, 18, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 5}), "Power Glove", 7, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 6}), "Bucket", 25, 18, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 7}), "Bow", 8, 21, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 8}), "Magic Compass", 14, 21, true);

    // food
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 0}), "Flesh", 8, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 1}), "Bread", 9, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 2}), "Apple", 10, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 3}), "Golden Apple", 22, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 4}), "Raw Pork", 10, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 5}), "Cooked Pork", 11, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 6}), "Raw Beef", 12, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 7}), "Steak", 13, 20, false);

    // furniture
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 0}), "Anvil", 15, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 1}), "Chest", 16, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 2}), "Oven", 17, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 3}), "Furnace", 18, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 4}), "Workbench", 19, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 5}), "Lantern", 20, 19, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 6}), "Loom", 15, 20, true);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 7}), "Enchanter", 18, 20, true);

    // items
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 1}), "Flower", 0, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), "Wood", 1, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), "Stone", 2, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 4}), "Sand", 27, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 5}), "Dirt", 28, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 6}), "Cloud", 29, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 7}), "Acorn", 3, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 8}), "Cactus", 4, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 9}), "Seeds", 5, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 10}), "Wheat", 6, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 11}), "Slime", 31, 18, false); 
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 12}), "Coal", 30, 18, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 13}), "Iron Ore", 11, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 14}), "Gold Ore", 30, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), "Iron Ingot", 12, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), "Gold Ingot", 31, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 17}), "Glass", 13, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), "Gem", 14, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 19}), "Wood Wall", 28, 18, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 20}), "Stone Wall", 28, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 21}), "Iron Wall", 29, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 22}), "Gold Wall", 30, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 23}), "Gem Wall", 31, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 24}), "Wool", 8, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), "String", 9, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 26}), "Leather", 14, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 27}), "Wood Arrow", 9, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 28}), "Stone Arrow", 10, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 29}), "Iron Arrow", 11, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 30}), "Gold Arrow", 12, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 31}), "Gem Arrow", 13, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 32}), "Bone", 16, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 33}), "Dungeonkey", 17, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 34}), "Wizard Summon", 19, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 35}), "Dragon Egg", 20, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 36}), "Dragon Scale", 21, 20, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 37}), "Bookshelves", 29, 18, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 38}), "Magic Dust", 25, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), "Coin", 26, 19, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 40}), "Undying", 18, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 41}), "Regeneration", 18, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 42}), "Speed", 18, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 43}), "Strength", 18, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 44}), "Shielding", 18, 21, false);
    _itemRegister(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 45}), "Night Vision", 18, 21, false);
}

char *itemGetName(int id, int countLevel) {
    switch (id) {
    // handle special cases here
    case TOOL_SHOVEL:
        switch (countLevel) {
        case 1:
            return "Rock Shovel";
        case 2:
            return "Iron Shovel";
        case 3:
            return "Gold Shovel";
        case 4:
            return "Gem Shovel";
        default:
            return "Wood Shovel";
        }
    case TOOL_HOE:
        switch (countLevel) {
        case 1:
            return "Rock Hoe";
        case 2:
            return "Iron Hoe";
        case 3:
            return "Gold Hoe";
        case 4:
            return "Gem Hoe";
        default:
            return "Wood Hoe";
        }
    case TOOL_SWORD:
        switch (countLevel) {
        case 1:
            return "Rock Sword";
        case 2:
            return "Iron Sword";
        case 3:
            return "Gold Sword";
        case 4:
            return "Gem Sword";
        default:
            return "Wood Sword";
        }
    case TOOL_PICKAXE:
        switch (countLevel) {
        case 1:
            return "Rock Pickaxe";
        case 2:
            return "Iron Pickaxe";
        case 3:
            return "Gold Pickaxe";
        case 4:
            return "Gem Pickaxe";
        default:
            return "Wood Pickaxe";
        }
    case TOOL_AXE:
        switch (countLevel) {
        case 1:
            return "Rock Axe";
        case 2:
            return "Iron Axe";
        case 3:
            return "Gold Axe";
        case 4:
            return "Gem Axe";
        default:
            return "Wood Axe";
        }
    case TOOL_BUCKET:
        switch (countLevel) {
        case 1:
            return "Water Bucket";
        case 2:
            return "Lava Bucket";
        default:
            return "Empty Bucket";
        }
    // else just return the registered value
    default:
        return _itemNames[id];
    }
}

char _itemCurrentName[20];
char *itemGetNameWithCount(int id, int countLevel) {
    if (itemIsSingle(id, countLevel)) {
        return itemGetName(id, countLevel);
    } else {
        sprintf(_itemCurrentName, "%d %s", countLevel, itemGetName(id, countLevel));
        return _itemCurrentName;
    }
}

int itemGetIconX(int id, int countLevel) {
    switch (id) {
    // handle special cases here
    case TOOL_SHOVEL:
    case TOOL_HOE:
    case TOOL_SWORD:
    case TOOL_PICKAXE:
    case TOOL_AXE:
    case TOOL_BUCKET:
        return _itemIconX[id] + countLevel;
    // else just return the registered value
    default:
        return _itemIconX[id];
    }
}

int itemGetIconY(int id, int countLevel) {
    switch (id) {
    // handle special cases here

    // else just return the registered value
    default:
        return _itemIconY[id];
    }
}

bool itemIsSingle(int id, int countLevel) {
    return _itemSingle[id];
}
