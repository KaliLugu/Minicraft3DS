#include "items/ItemsData.h"
#include "items/ItemsTypes.h"
#include <stdio.h>

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

// tableau temporaire de mapping des items pour la migration vers le nouveau système d'item
static ItemID g_legacyToNew[MAX_ITEM_ID + 1];

static char *_itemNames[MAX_ITEM_ID + 1];
static int _itemIconX[MAX_ITEM_ID + 1];
static int _itemIconY[MAX_ITEM_ID + 1];
static bool _itemSingle[MAX_ITEM_ID + 1];

static void _itemRegister(int id, char *name, int iconX, int iconY, bool isSingle) {
    _itemNames[id] = name;
    _itemIconX[id] = iconX; // sur la largeur
    _itemIconY[id] = iconY; // sur la hauteur
    _itemSingle[id] = isSingle;
}

void itemsDataInit() {

    _itemRegister(ITEM_NULL, "", 0, 0, true);

    // tools 1
    _itemRegister(TOOL_SHOVEL, "Shovel", 0, 18, true);
    _itemRegister(TOOL_HOE, "Hoe", 5, 18, true);
    _itemRegister(TOOL_SWORD, "Sword", 10, 18, true);
    _itemRegister(TOOL_PICKAXE, "Pickaxe", 15, 18, true);
    _itemRegister(TOOL_AXE, "Axe", 20, 18, true);
    _itemRegister(ITEM_POWGLOVE, "Power Glove", 7, 19, true);

    // items 1
    _itemRegister(ITEM_FLOWER, "Flower", 0, 19, false);
    _itemRegister(ITEM_WOOD, "Wood", 1, 19, false);
    _itemRegister(ITEM_STONE, "Stone", 2, 19, false);
    _itemRegister(ITEM_SAND, "Sand", 27, 19, false);
    _itemRegister(ITEM_DIRT, "Dirt", 28, 19, false);
    _itemRegister(ITEM_CLOUD, "Cloud", 29, 19, false);
    _itemRegister(ITEM_ACORN, "Acorn", 3, 19, false);
    _itemRegister(ITEM_CACTUS, "Cactus", 4, 19, false);
    _itemRegister(ITEM_SEEDS, "Seeds", 5, 19, false);
    _itemRegister(ITEM_WHEAT, "Wheat", 6, 19, false);
    _itemRegister(ITEM_FLESH, "Flesh", 8, 19, false);
    _itemRegister(ITEM_BREAD, "Bread", 9, 19, false);
    _itemRegister(ITEM_GOLDEN_APPLE, "Golden Apple", 22, 20, false);
    _itemRegister(ITEM_APPLE, "Apple", 10, 19, false);
    _itemRegister(ITEM_SLIME, "Slime", 31, 18, false);
    _itemRegister(ITEM_COAL, "Coal", 30, 18, false);
    _itemRegister(ITEM_IRONORE, "Iron Ore", 11, 19, false);
    _itemRegister(ITEM_GOLDORE, "Gold Ore", 30, 19, false);
    _itemRegister(ITEM_IRONINGOT, "Iron Ingot", 12, 19, false);
    _itemRegister(ITEM_GOLDINGOT, "Gold Ingot", 31, 19, false);
    _itemRegister(ITEM_GLASS, "Glass", 13, 19, false);
    _itemRegister(ITEM_GEM, "Gem", 14, 19, false);

    // furniture
    _itemRegister(ITEM_ANVIL, "Anvil", 15, 19, true);
    _itemRegister(ITEM_CHEST, "Chest", 16, 19, true);
    _itemRegister(ITEM_OVEN, "Oven", 17, 19, true);
    _itemRegister(ITEM_FURNACE, "Furnace", 18, 19, true);
    _itemRegister(ITEM_WORKBENCH, "Workbench", 19, 19, true);
    _itemRegister(ITEM_LANTERN, "Lantern", 20, 19, true);
    _itemRegister(ITEM_LOOM, "Loom", 15, 20, true);
    _itemRegister(ITEM_ENCHANTER, "Enchanter", 18, 20, true);

    // items 2
    _itemRegister(ITEM_WALL_WOOD, "Wood Wall", 28, 18, false);
    _itemRegister(ITEM_WALL_STONE, "Stone Wall", 28, 20, false);
    _itemRegister(ITEM_WALL_IRON, "Iron Wall", 29, 20, false);
    _itemRegister(ITEM_WALL_GOLD, "Gold Wall", 30, 20, false);
    _itemRegister(ITEM_WALL_GEM, "Gem Wall", 31, 20, false);
    _itemRegister(ITEM_WOOL, "Wool", 8, 20, false);
    _itemRegister(ITEM_STRING, "String", 9, 20, false);
    _itemRegister(ITEM_PORK_RAW, "Raw Pork", 10, 20, false);
    _itemRegister(ITEM_PORK_COOKED, "Cooked Pork", 11, 20, false);
    _itemRegister(ITEM_BEEF_RAW, "Raw Beef", 12, 20, false);
    _itemRegister(ITEM_BEEF_COOKED, "Steak", 13, 20, false);
    _itemRegister(ITEM_LEATHER, "Leather", 14, 20, false);
    _itemRegister(ITEM_ARROW_WOOD, "Wood Arrow", 9, 21, false);
    _itemRegister(ITEM_ARROW_STONE, "Stone Arrow", 10, 21, false);
    _itemRegister(ITEM_ARROW_IRON, "Iron Arrow", 11, 21, false);
    _itemRegister(ITEM_ARROW_GOLD, "Gold Arrow", 12, 21, false);
    _itemRegister(ITEM_ARROW_GEM, "Gem Arrow", 13, 21, false);
    _itemRegister(ITEM_BONE, "Bone", 16, 20, false);
    _itemRegister(ITEM_DUNGEON_KEY, "Dungeonkey", 17, 20, false);
    _itemRegister(ITEM_WIZARD_SUMMON, "Wizard Summon", 19, 20, false);
    _itemRegister(ITEM_DRAGON_EGG, "Dragon Egg", 20, 20, false);
    _itemRegister(ITEM_DRAGON_SCALE, "Dragon Scale", 21, 20, false);
    _itemRegister(ITEM_BOOKSHELVES, "Bookshelves", 29, 18, false);
    _itemRegister(ITEM_MAGIC_DUST, "Magic Dust", 25, 19, false);
    _itemRegister(ITEM_COIN, "Coin", 26, 19, false);

    // tools 2
    _itemRegister(TOOL_BUCKET, "Bucket", 25, 18, true);
    _itemRegister(TOOL_BOW, "Bow", 8, 21, true);
    _itemRegister(TOOL_MAGIC_COMPASS, "Magic Compass", 14, 21, true);

    // items 3
    _itemRegister(ITEM_SCROLL_UNDYING, "Undying", 18, 21, false);
    _itemRegister(ITEM_SCROLL_REGENERATION, "Regeneration", 18, 21, false);
    _itemRegister(ITEM_SCROLL_SPEED, "Speed", 18, 21, false);
    _itemRegister(ITEM_SCROLL_STRENGTH, "Strength", 18, 21, false);
    _itemRegister(ITEM_SCROLL_SHIELDING, "Shielding", 18, 21, false);
    _itemRegister(ITEM_SCROLL_NIGHTVISION, "Night Vision", 18, 21, false);
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
