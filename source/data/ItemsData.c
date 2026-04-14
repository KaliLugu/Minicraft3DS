#include "items/ItemsData.h"
#include "items/ItemsTypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int *_itemIconX = NULL;
static int *_itemIconY = NULL;

ItemId getIdFromName(const char *name) {
    if (name == NULL) return g_itemTable[0].id;
    for (unsigned int i = 0; i < g_itemCount; ++i) {
        if (strcmp(g_itemTable[i].name, name) == 0) {
            return g_itemTable[i].id;
        }
    }
    // si pas trouvé, retourner l'id du premier item (NULL)
    return g_itemTable[0].id;
}

const char* getNameFromId(ItemId id) {
    if (id > g_itemCount - 1) return "NULL"; // bounds check (supprimer id < 0)
    for (unsigned int i = 0; i < g_itemCount; ++i) {
        if (g_itemTable[i].id == id) {
            return g_itemTable[i].name;
        }
    }
    return "null";
}

unsigned int getToolCountLevel(ItemId id) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    if (g_itemTable[id].category != ITEM_CAT_TOOL) return 0; // not tool
    return g_itemTable[id].data.tool.countLevel;
}

unsigned int getFoodHealth(ItemId id) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    if (g_itemTable[id].category != ITEM_CAT_FOOD) return 0; // not food
    return g_itemTable[id].data.food.health;
}

unsigned int getSpellDuration(ItemId id) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    if (g_itemTable[id].category != ITEM_CAT_SPELL) return 0; // not spell
    return g_itemTable[id].data.spell.duration;
}

unsigned int getSpellEffect(ItemId id) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    if (g_itemTable[id].category != ITEM_CAT_SPELL) return 0; // not spell
    return g_itemTable[id].data.spell.effect;
}

unsigned int getItemCategory(ItemId id) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    return g_itemTable[id].category;
}

static void _itemRegister(int id, char *name, int iconX, int iconY, bool isSingle) {
    g_itemTable[id].displayName = name;
    _itemIconX[id] = iconX; // sur la largeur
    _itemIconY[id] = iconY; // sur la hauteur
}

void itemsDataInit() {
    _itemIconX = calloc(g_itemCount, sizeof(int));
    _itemIconY = calloc(g_itemCount, sizeof(int));

    _itemRegister(getIdFromName("NULL"), "", 0, 0, true);

    // tools
    _itemRegister(getIdFromName("TOOL_SHOVEL"), "Shovel", 0, 18, true);
    _itemRegister(getIdFromName("TOOL_HOE"), "Hoe", 5, 18, true);
    _itemRegister(getIdFromName("TOOL_SWORD"), "Sword", 10, 18, true);
    _itemRegister(getIdFromName("TOOL_PICKAXE"), "Pickaxe", 15, 18, true);
    _itemRegister(getIdFromName("TOOL_AXE"), "Axe", 20, 18, true);
    _itemRegister(getIdFromName("ITEM_POWGLOVE"), "Power Glove", 7, 19, true);
    _itemRegister(getIdFromName("TOOL_BUCKET"), "Bucket", 25, 18, true);
    _itemRegister(getIdFromName("ITEM_BOW"), "Bow", 8, 21, true);
    _itemRegister(getIdFromName("ITEM_MAGIC_COMPASS"), "Magic Compass", 14, 21, true);

    // food
    _itemRegister(getIdFromName("ITEM_FLESH"), "Flesh", 8, 19, false);
    _itemRegister(getIdFromName("ITEM_BREAD"), "Bread", 9, 19, false);
    _itemRegister(getIdFromName("ITEM_APPLE"), "Apple", 10, 19, false);
    _itemRegister(getIdFromName("ITEM_GOLDEN_APPLE"), "Golden Apple", 22, 20, false);
    _itemRegister(getIdFromName("ITEM_PORK_RAW"), "Raw Pork", 10, 20, false);
    _itemRegister(getIdFromName("ITEM_PORK_COOKED"), "Cooked Pork", 11, 20, false);
    _itemRegister(getIdFromName("ITEM_BEEF_RAW"), "Raw Beef", 12, 20, false);
    _itemRegister(getIdFromName("ITEM_BEEF_COOKED"), "Cooked Beef", 13, 20, false);

    // furniture
    _itemRegister(getIdFromName("ITEM_ANVIL"), "Anvil", 15, 19, true);
    _itemRegister(getIdFromName("ITEM_CHEST"), "Chest", 16, 19, true);
    _itemRegister(getIdFromName("ITEM_OVEN"), "Oven", 17, 19, true);
    _itemRegister(getIdFromName("ITEM_FURNACE"), "Furnace", 18, 19, true);
    _itemRegister(getIdFromName("ITEM_WORKBENCH"), "Workbench", 19, 19, true);
    _itemRegister(getIdFromName("ITEM_LANTERN"), "Lantern", 20, 19, true);
    _itemRegister(getIdFromName("ITEM_LOOM"), "Loom", 15, 20, true);
    _itemRegister(getIdFromName("ITEM_ENCHANTER"), "Enchanter", 18, 20, true);

    // items
    _itemRegister(getIdFromName("ITEM_FLOWER"), "Flower", 0, 19, false);
    _itemRegister(getIdFromName("ITEM_WOOD"), "Wood", 1, 19, false);
    _itemRegister(getIdFromName("ITEM_STONE"), "Stone", 2, 19, false);
    _itemRegister(getIdFromName("ITEM_SAND"), "Sand", 27, 19, false);
    _itemRegister(getIdFromName("ITEM_DIRT"), "Dirt", 28, 19, false);
    _itemRegister(getIdFromName("ITEM_CLOUD"), "Cloud", 29, 19, false);
    _itemRegister(getIdFromName("ITEM_ACORN"), "Acorn", 3, 19, false);
    _itemRegister(getIdFromName("ITEM_CACTUS"), "Cactus", 4, 19, false);
    _itemRegister(getIdFromName("ITEM_SEED"), "Seed", 5, 19, false);
    _itemRegister(getIdFromName("ITEM_WHEAT"), "Wheat", 6, 19, false);
    _itemRegister(getIdFromName("ITEM_SLIME"), "Slime", 31, 18, false);
    _itemRegister(getIdFromName("ITEM_COAL"), "Coal", 30, 18, false);
    _itemRegister(getIdFromName("ITEM_IRONORE"), "Iron Ore", 11, 19, false);
    _itemRegister(getIdFromName("ITEM_GOLDORE"), "Gold Ore", 30, 19, false);
    _itemRegister(getIdFromName("ITEM_IRONINGOT"), "Iron Ingot", 12, 19, false);
    _itemRegister(getIdFromName("ITEM_GOLDINGOT"), "Gold Ingot", 31, 19, false);
    _itemRegister(getIdFromName("ITEM_GLASS"), "Glass", 13, 19, false);
    _itemRegister(getIdFromName("ITEM_GEM"), "Gem", 14, 19, false);
    _itemRegister(getIdFromName("ITEM_WALL_WOOD"), "Wood Wall", 28, 18, false);
    _itemRegister(getIdFromName("ITEM_WALL_STONE"), "Stone Wall", 28, 20, false);
    _itemRegister(getIdFromName("ITEM_WALL_IRON"), "Iron Wall", 29, 20, false);
    _itemRegister(getIdFromName("ITEM_WALL_GOLD"), "Gold Wall", 30, 20, false);
    _itemRegister(getIdFromName("ITEM_WALL_GEM"), "Gem Wall", 31, 20, false);
    _itemRegister(getIdFromName("ITEM_WOOL"), "Wool", 8, 20, false);
    _itemRegister(getIdFromName("ITEM_STRING"), "String", 9, 20, false);
    _itemRegister(getIdFromName("ITEM_LEATHER"), "Leather", 14, 20, false);
    _itemRegister(getIdFromName("ITEM_ARROW_WOOD"), "Wood Arrow", 9, 21, false);
    _itemRegister(getIdFromName("ITEM_ARROW_STONE"), "Stone Arrow", 10, 21, false);
    _itemRegister(getIdFromName("ITEM_ARROW_IRON"), "Iron Arrow", 11, 21, false);
    _itemRegister(getIdFromName("ITEM_ARROW_GOLD"), "Gold Arrow", 12, 21, false);
    _itemRegister(getIdFromName("ITEM_ARROW_GEM"), "Gem Arrow", 13, 21, false);
    _itemRegister(getIdFromName("ITEM_BONE"), "Bone", 16, 20, false);
    _itemRegister(getIdFromName("ITEM_DUNGEON_KEY"), "Dungeon Key", 17, 20, false);
    _itemRegister(getIdFromName("ITEM_WIZARD_SUMMON"), "Wizard Summon", 19, 20, false);
    _itemRegister(getIdFromName("ITEM_DRAGON_EGG"), "Dragon Egg", 20, 20, false);
    _itemRegister(getIdFromName("ITEM_DRAGON_SCALE"), "Dragon Scale", 21, 20, false);
    _itemRegister(getIdFromName("ITEM_BOOKSHELVES"), "Bookshelves", 29, 18, false);
    _itemRegister(getIdFromName("ITEM_MAGIC_DUST"), "Magic Dust", 25, 19, false);
    _itemRegister(getIdFromName("ITEM_COIN"), "Coin", 26, 19, false);
    _itemRegister(getIdFromName("ITEM_SCROLL_UNDYING"), "Scroll of Undying", 18, 21, false);
    _itemRegister(getIdFromName("ITEM_SCROLL_REGENERATION"), "Scroll of Regeneration", 18, 21, false);
    _itemRegister(getIdFromName("ITEM_SCROLL_SPEED"), "Scroll of Speed", 18, 21, false);
    _itemRegister(getIdFromName("ITEM_SCROLL_STRENGTH"), "Scroll of Strength", 18, 21, false);
    _itemRegister(getIdFromName("ITEM_SCROLL_SHIELDING"), "Scroll of Shielding", 18, 21, false);
    _itemRegister(getIdFromName("ITEM_SCROLL_NIGHTVISION"), "Scroll of Nightvision", 18, 21, false);
}

char *itemGetName(int id, int countLevel) {
    if (id < 0 || id >= g_itemCount) return "";
    if (getItemCategory(id) == ITEM_CAT_TOOL) {
        if (id == getIdFromName("TOOL_SHOVEL")) {
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
        } else if (id == getIdFromName("TOOL_HOE")) {
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
        } else if (id == getIdFromName("TOOL_SWORD")) {
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
        } else if (id == getIdFromName("TOOL_PICKAXE")) {
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
        } else if (id == getIdFromName("TOOL_AXE")) {
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
        } else if (id == getIdFromName("TOOL_BUCKET")) {
            switch (countLevel) {
            case 1:
                return "Water Bucket";
            case 2:
                return "Lava Bucket";
            default:
                return "Empty Bucket";
            }
        }
    }
    return g_itemTable[id].displayName;
}

char _itemCurrentName[32];
char *itemGetNameWithCount(int id, int countLevel) {
    if (itemIsSingle(id, countLevel)) {
        return itemGetName(id, countLevel);
    } else {
        sprintf(_itemCurrentName, "%d %s", countLevel, itemGetName(id, countLevel));
        return _itemCurrentName;
    }
}

int itemGetIconX(int id, int countLevel) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    if (id == getIdFromName("TOOL_SHOVEL") ||
        id == getIdFromName("TOOL_HOE") ||
        id == getIdFromName("TOOL_SWORD") ||
        id == getIdFromName("TOOL_PICKAXE") ||
        id == getIdFromName("TOOL_AXE") ||
        id == getIdFromName("TOOL_BUCKET")) {
        // handle special cases here
        return _itemIconX[id] + countLevel;
    } else return _itemIconX[id];
}

int itemGetIconY(int id, int countLevel) {
    if (id < 0 || id >= g_itemCount) return 0; // bounds check
    switch (id) {
    // handle special cases here

    // else just return the registered value
    default:
        return _itemIconY[id];
    }
}

bool itemIsSingle(int id, int countLevel) {
    if (id < 0 || id >= g_itemCount) return true; // bounds check
    return !g_itemTable[id].isStackable;
}
