#pragma once
#include "../engine/dtypes.h"
#include <stdint.h>

// TODO HERE 
//
// mettre une boucle for pour générer automatiquement les id des items et ne plus devoir les setup a la main
// commencé a écrire une doc sur le futur système d'item dans le doc "docs/ItemSystem.md"

typedef struct { // structure pour définir un item
    const char* base_name;
    const char** level_names;
    unsigned int level_count;
} Item;

typedef struct { // structure pour les catégorie d'items
    Item* items;
    unsigned int count;
} ItemRegistry;

typedef struct { // structure pour identifier un item
    uint8_t category;  // 0 = NULL
    uint16_t id;       // index dans la catégorie
} ItemID;

//fr: catégories d'items
//eng: item categories
#define TOOL 1
#define FURNITURE 2
#define ITEM 3
#define FOOD 4

#define NEW_ITEM_NULL ((ItemID){0, 0})

// tools 1
#define NEW_TOOL_SHOVEL ((ItemID){TOOL, 0})
#define NEW_TOOL_HOE ((ItemID){TOOL, 1})
#define NEW_TOOL_SWORD ((ItemID){TOOL, 2})
#define NEW_TOOL_PICKAXE ((ItemID){TOOL, 3})
#define NEW_TOOL_AXE ((ItemID){TOOL, 4})
#define NEW_ITEM_POWGLOVE ((ItemID){TOOL, 5})

// tools 2
#define NEW_TOOL_BUCKET ((ItemID){TOOL, 6})
#define NEW_TOOL_BOW ((ItemID){TOOL, 7})
#define NEW_TOOL_MAGIC_COMPASS ((ItemID){TOOL, 8})

// furniture
#define ITEM_ANVIL ((ItemID){FURNITURE, 0})
#define ITEM_CHEST ((ItemID){FURNITURE, 1})
#define ITEM_OVEN ((ItemID){FURNITURE, 2})
#define ITEM_FURNACE ((ItemID){FURNITURE, 3})
#define ITEM_WORKBENCH ((ItemID){FURNITURE, 4})
#define ITEM_LANTERN ((ItemID){FURNITURE, 5})
#define ITEM_LOOM ((ItemID){FURNITURE, 6})
#define ITEM_ENCHANTER ((ItemID){FURNITURE, 7})

// items 1
#define NEW_ITEM_FLOWER ((ItemID){ITEM, 0})
#define NEW_ITEM_WOOD ((ItemID){ITEM, 1})
#define NEW_ITEM_STONE ((ItemID){ITEM, 2})
#define NEW_ITEM_SAND ((ItemID){ITEM, 3})
#define NEW_ITEM_DIRT ((ItemID){ITEM, 4})
#define NEW_ITEM_CLOUD ((ItemID){ITEM, 5})
#define NEW_ITEM_ACORN ((ItemID){ITEM, 6})
#define NEW_ITEM_CACTUS ((ItemID){ITEM, 7})
#define NEW_ITEM_SEEDS ((ItemID){ITEM, 8})
#define NEW_ITEM_WHEAT ((ItemID){ITEM, 9})
#define NEW_ITEM_SLIME ((ItemID){ITEM, 10})
#define NEW_ITEM_COAL ((ItemID){ITEM, 11})
#define NEW_ITEM_IRONORE ((ItemID){ITEM, 12})
#define NEW_ITEM_GOLDORE ((ItemID){ITEM, 13})
#define NEW_ITEM_IRONINGOT ((ItemID){ITEM, 14})
#define NEW_ITEM_GOLDINGOT ((ItemID){ITEM, 15})
#define NEW_ITEM_GLASS ((ItemID){ITEM, 16})
#define NEW_ITEM_GEM ((ItemID){ITEM, 17})
#define NEW_ITEM_GOLDEN_APPLE ((ItemID){ITEM, 18})

#define NEW_ITEM_FLESH ((ItemID){FOOD, 0})
#define NEW_ITEM_BREAD ((ItemID){FOOD, 1})
#define NEW_ITEM_APPLE ((ItemID){FOOD, 2})
#define NEW_ITEM_GOLDEN_APPLE ((ItemID){ITEM, 3})

// NOTE: Items 0-6, 28-50, 101-1000 are for unstackable (tools, furniture) items
#define ITEM_NULL 0

// tools 1
#define TOOL_SHOVEL 1
#define TOOL_HOE 2
#define TOOL_SWORD 3
#define TOOL_PICKAXE 4
#define TOOL_AXE 5
#define ITEM_POWGLOVE 6

// items 1
#define ITEM_FLOWER 7
#define ITEM_WOOD 8
#define ITEM_STONE 9
#define ITEM_SAND 10
#define ITEM_DIRT 11
#define ITEM_CLOUD 12
#define ITEM_ACORN 13
#define ITEM_CACTUS 14
#define ITEM_SEEDS 15
#define ITEM_WHEAT 16
#define ITEM_FLESH 17
#define ITEM_BREAD 18
#define ITEM_APPLE 19
#define ITEM_SLIME 20
#define ITEM_COAL 21
#define ITEM_IRONORE 22
#define ITEM_GOLDORE 23
#define ITEM_IRONINGOT 24
#define ITEM_GOLDINGOT 25
#define ITEM_GLASS 26
#define ITEM_GEM 27
#define ITEM_GOLDEN_APPLE 1007

// furniture
#define ITEM_ANVIL 28
#define ITEM_CHEST 29
#define ITEM_OVEN 30
#define ITEM_FURNACE 31
#define ITEM_WORKBENCH 32
#define ITEM_LANTERN 33
#define ITEM_LOOM 34
#define ITEM_ENCHANTER 35

// items 2
#define ITEM_WALL_WOOD 51
#define ITEM_WALL_STONE 52
#define ITEM_WALL_IRON 53
#define ITEM_WALL_GOLD 54
#define ITEM_WALL_GEM 55
#define ITEM_WOOL 56
#define ITEM_STRING 57
#define ITEM_PORK_RAW 58
#define ITEM_PORK_COOKED 59
#define ITEM_BEEF_RAW 60
#define ITEM_BEEF_COOKED 61
#define ITEM_LEATHER 62
#define ITEM_ARROW_WOOD 63
#define ITEM_ARROW_STONE 64
#define ITEM_ARROW_IRON 65
#define ITEM_ARROW_GOLD 66
#define ITEM_ARROW_GEM 67
#define ITEM_BONE 68
#define ITEM_DUNGEON_KEY 69
#define ITEM_WIZARD_SUMMON 70
#define ITEM_DRAGON_EGG 71
#define ITEM_DRAGON_SCALE 72
#define ITEM_BOOKSHELVES 73
#define ITEM_MAGIC_DUST 74
#define ITEM_COIN 75

// tools 2
#define TOOL_BUCKET 101
#define TOOL_BOW 102
#define TOOL_MAGIC_COMPASS 103

// items 3
#define ITEM_SCROLL_UNDYING 1001
#define ITEM_SCROLL_REGENERATION 1002
#define ITEM_SCROLL_SPEED 1003
#define ITEM_SCROLL_STRENGTH 1004
#define ITEM_SCROLL_SHIELDING 1005
#define ITEM_SCROLL_NIGHTVISION 1006

#define MAX_ITEM_ID 1007

extern void itemsDataInit();

extern char *itemGetName(int id, int countLevel);
extern char *itemGetNameWithCount(int id, int countLevel);
extern int itemGetIconX(int id, int countLevel);
extern int itemGetIconY(int id, int countLevel);
extern bool itemIsSingle(int id, int countLevel);
