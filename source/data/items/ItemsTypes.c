#include "ItemsData.h"
#include <stdlib.h>

#define ITEM_ENTRY(name, displayname, cat, stackable, texX, texY, ...) \
    {0, name, displayname, cat, stackable, texX, texY, __VA_ARGS__}

/* Vanilla items definitions */
static const ItemData _vanillaDefs[] = {
    // name, display name, cat, stackable, texX, texY, data
    ITEM_ENTRY("NULL", "", ITEM_CAT_GENERIC, false, 0, 0, {.generic = {false}}),
    ITEM_ENTRY("TOOL_SHOVEL", "Shovel", ITEM_CAT_TOOL, false, 0, 18, {.tool = {0}}),
    ITEM_ENTRY("TOOL_HOE", "Hoe", ITEM_CAT_TOOL, false, 5, 18, {.tool = {0}}),
    ITEM_ENTRY("TOOL_SWORD", "Sword", ITEM_CAT_TOOL, false, 10, 18, {.tool = {0}}),
    ITEM_ENTRY("TOOL_PICKAXE", "Pickaxe", ITEM_CAT_TOOL, false, 15, 18, {.tool = {0}}),
    ITEM_ENTRY("TOOL_AXE", "Axe", ITEM_CAT_TOOL, false, 20, 18, {.tool = {0}}),
    ITEM_ENTRY("ITEM_POWGLOVE", "Power Glove", ITEM_CAT_GENERIC, false, 7, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_FLOWER", "Flower", ITEM_CAT_GENERIC, true, 0, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_WOOD", "Wood", ITEM_CAT_GENERIC, true, 1, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_STONE", "Stone", ITEM_CAT_GENERIC, true, 2, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_SAND", "Sand", ITEM_CAT_GENERIC, true, 27, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_DIRT", "Dirt", ITEM_CAT_GENERIC, true, 28, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_CLOUD", "Cloud", ITEM_CAT_GENERIC, true, 29, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_ACORN", "Acorn", ITEM_CAT_GENERIC, true, 3, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_CACTUS", "Cactus", ITEM_CAT_GENERIC, true, 4, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_SEED", "Seed", ITEM_CAT_GENERIC, true, 5, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_WHEAT", "Wheat", ITEM_CAT_GENERIC, true, 6, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_FLESH", "Flesh", ITEM_CAT_FOOD, true, 8, 19, {.food = {1}}),
    ITEM_ENTRY("ITEM_BREAD", "Bread", ITEM_CAT_FOOD, true, 9, 19, {.food = {2}}),
    ITEM_ENTRY("ITEM_APPLE", "Apple", ITEM_CAT_FOOD, true, 10, 19, {.food = {1}}),
    ITEM_ENTRY("ITEM_SLIME", "Slime", ITEM_CAT_GENERIC, true, 31, 18, {.generic = {false}}),
    ITEM_ENTRY("ITEM_COAL", "Coal", ITEM_CAT_GENERIC, true, 30, 18, {.generic = {false}}),
    ITEM_ENTRY("ITEM_IRONORE", "Iron Ore", ITEM_CAT_GENERIC, true, 11, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_GOLDORE", "Gold Ore", ITEM_CAT_GENERIC, true, 30, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_IRONINGOT", "Iron Ingot", ITEM_CAT_GENERIC, true, 12, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_GOLDINGOT", "Gold Ingot", ITEM_CAT_GENERIC, true, 31, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_GLASS", "Glass", ITEM_CAT_GENERIC, true, 13, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_GEM", "Gem", ITEM_CAT_GENERIC, true, 14, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_GOLDEN_APPLE", "Golden Apple", ITEM_CAT_FOOD, true, 22, 20, {.food = {7}}),
    ITEM_ENTRY("ITEM_ANVIL", "Anvil", ITEM_CAT_FURNITURE, false, 15, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_CHEST", "Chest", ITEM_CAT_FURNITURE, false, 16, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_OVEN", "Oven", ITEM_CAT_FURNITURE, false, 17, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_FURNACE", "Furnace", ITEM_CAT_FURNITURE, false, 18, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_WORKBENCH", "Workbench", ITEM_CAT_FURNITURE, false, 19, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_LANTERN", "Lantern", ITEM_CAT_FURNITURE, false, 20, 19, {.generic = {false}}),
    ITEM_ENTRY("ITEM_LOOM", "Loom", ITEM_CAT_FURNITURE, false, 15, 20, {.generic = {false}}),
    ITEM_ENTRY("ITEM_ENCHANTER", "Enchanter", ITEM_CAT_FURNITURE, false, 18, 20, {.generic = {false}}),
    ITEM_ENTRY("ITEM_WALL_WOOD", "Wall Wood", ITEM_CAT_GENERIC, true, 28, 18, {}),
    ITEM_ENTRY("ITEM_WALL_STONE", "Wall Stone", ITEM_CAT_GENERIC, true,28, 20, {}),
    ITEM_ENTRY("ITEM_WALL_IRON", "Wall Iron", ITEM_CAT_GENERIC, true, 29, 20, {}),
    ITEM_ENTRY("ITEM_WALL_GOLD", "Wall Gold", ITEM_CAT_GENERIC, true, 30, 20, {}),
    ITEM_ENTRY("ITEM_WALL_GEM", "Wall Gem", ITEM_CAT_GENERIC, true, 31, 20, {}),
    ITEM_ENTRY("ITEM_WOOL", "Wool", ITEM_CAT_GENERIC, true, 8, 20, {}),
    ITEM_ENTRY("ITEM_STRING", "String", ITEM_CAT_GENERIC, true, 9, 20, {}),
    ITEM_ENTRY("ITEM_PORK_RAW", "Raw Pork", ITEM_CAT_FOOD, true, 10, 20, {.food = {1}}),
    ITEM_ENTRY("ITEM_PORK_COOKED", "Cooked Pork", ITEM_CAT_FOOD, true, 11, 20, {.food = {3}}),
    ITEM_ENTRY("ITEM_BEEF_RAW", "Raw Beef", ITEM_CAT_FOOD, true, 12, 20, {.food = {1}}),
    ITEM_ENTRY("ITEM_BEEF_COOKED", "Cooked Beef", ITEM_CAT_FOOD, true, 13, 20, {.food = {4}}),
    ITEM_ENTRY("ITEM_LEATHER", "Leather", ITEM_CAT_GENERIC, true, 14, 20, {}),
    ITEM_ENTRY("ITEM_ARROW_WOOD", "Arrow Wood", ITEM_CAT_GENERIC, true, 9, 21, {}),
    ITEM_ENTRY("ITEM_ARROW_STONE", "Arrow Stone", ITEM_CAT_GENERIC, true, 10, 21, {}),
    ITEM_ENTRY("ITEM_ARROW_IRON", "Arrow Iron", ITEM_CAT_GENERIC, true, 11, 21, {}),
    ITEM_ENTRY("ITEM_ARROW_GOLD", "Arrow Gold", ITEM_CAT_GENERIC, true, 12, 21, {}),
    ITEM_ENTRY("ITEM_ARROW_GEM", "Arrow Gem", ITEM_CAT_GENERIC, true, 13, 21, {}),
    ITEM_ENTRY("ITEM_BONE", "Bone", ITEM_CAT_GENERIC, true, 16, 20, {}),
    ITEM_ENTRY("ITEM_DUNGEON_KEY", "Dungeon Key", ITEM_CAT_GENERIC, true, 17, 20, {}),
    ITEM_ENTRY("ITEM_WIZARD_SUMMON", "Wizard Summon", ITEM_CAT_GENERIC, true, 19, 20, {}),
    ITEM_ENTRY("ITEM_DRAGON_EGG", "Dragon Egg", ITEM_CAT_GENERIC, true, 20, 20, {}),
    ITEM_ENTRY("ITEM_DRAGON_SCALE", "Dragon Scale", ITEM_CAT_GENERIC, true, 21, 20, {}),
    ITEM_ENTRY("ITEM_BOOKSHELVES", "Bookshelves", ITEM_CAT_GENERIC, true, 29, 18,{}),
    ITEM_ENTRY("ITEM_MAGIC_DUST", "Magic Dust", ITEM_CAT_GENERIC, true, 25, 19, {}),
    ITEM_ENTRY("ITEM_COIN", "Coin", ITEM_CAT_GENERIC, true, 26, 19, {}),
    ITEM_ENTRY("TOOL_BUCKET", "Bucket", ITEM_CAT_TOOL, false, 25, 18, {.tool = {2}}),
    ITEM_ENTRY("ITEM_BOW", "Bow", ITEM_CAT_GENERIC, false, 8, 21, {}),
    ITEM_ENTRY("ITEM_MAGIC_COMPASS", "Magic Compass", ITEM_CAT_GENERIC, false, 14, 21, {}),
    ITEM_ENTRY("ITEM_SCROLL_UNDYING", "Scroll of Undying", ITEM_CAT_SPELL, true ,18, 21, {.spell = {5, 0} }),
    ITEM_ENTRY("ITEM_SCROLL_REGENERATION", "Scroll of Regeneration", ITEM_CAT_SPELL, true, 18, 21, {.spell = {5, 1}}),
    ITEM_ENTRY("ITEM_SCROLL_SPEED", "Scroll of Speed", ITEM_CAT_SPELL, true, 18, 21, {.spell = {5, 2}}),
    ITEM_ENTRY("ITEM_SCROLL_STRENGTH", "Scroll of Strength", ITEM_CAT_SPELL, true, 18, 21, {.spell = {5, 3}}),
    ITEM_ENTRY("ITEM_SCROLL_SHIELDING", "Scroll of Shielding", ITEM_CAT_SPELL, true, 18, 21, {.spell = {5, 4}}),
    ITEM_ENTRY("ITEM_SCROLL_NIGHTVISION", "Scroll of Nightvision", ITEM_CAT_SPELL, true, 18, 21, {.spell = {5, 5}}),
};

static const unsigned int _vanillaCount = sizeof(_vanillaDefs) / sizeof(_vanillaDefs[0]);

ItemData *g_itemTable = NULL;
unsigned int g_itemCount;



void itemsTableBuild(uint16_t modCount) {
    g_itemCount = _vanillaCount + modCount;
    g_itemTable = malloc(g_itemCount * sizeof(ItemData));
    if (!g_itemTable) {
        g_itemCount = 0;
        return;
    }
    for (unsigned int i = 0; i < _vanillaCount; i++) {
        g_itemTable[i] = _vanillaDefs[i];
        g_itemTable[i].id = (ItemId)i;
    }
}
