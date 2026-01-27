#include "items/ItemsData.h"

// new id tables, last column is legacy id
ToolItem toolItems[] = {
    {"Shovel", (const char*[]){"Wood Shovel", "Stone Shovel", "Gold Shovel", "Gem Shovel"}, 4, 1}, // 0
    {"Hoe", (const char*[]){"Wood Hoe", "Stone Hoe", "Gold Hoe", "Gem Hoe"}, 4, 2}, // 1
    {"Sword", (const char*[]){"Wood Sword", "Stone Sword", "Gold Sword", "Gem Sword"}, 4, 3}, // 2
    {"Pickaxe", (const char*[]){"Wood Pickaxe", "Stone Pickaxe", "Gold Pickaxe", "Gem Pickaxe"}, 4, 4}, // 3
    {"Axe", (const char*[]){"Wood Axe", "Stone Axe", "Gold Axe", "Gem Axe"}, 4, 5}, // 4
    {"Power Glove", (const char*[]){"Power Glove"}, 1, 6}, // 5
    {"Bucket", (const char*[]){"Empty Bucket", "Water Bucket", "Lava Bucket"}, 3, 101}, // 6
    {"Bow", (const char*[]){"Bow"}, 1, 102}, // 7
    {"Magic Compass", (const char*[]){"Magic Compass"}, 1, 103} // 8
};

FoodItem foodItems[] = {
    // index 0 for food item is first category of the table
    {"Flesh", 5, 17}, // 0
    {"Bread", 10, 18}, // 1
    {"Apple", 7, 19}, // 2
    {"Golden Apple", 20, 1007}, // 3
    {"Raw Pork", 8, 58}, // 4
    {"Cooked Pork", 14, 59}, // 5
    {"Raw Beef", 9, 60}, // 6
    {"Steak", 15, 61} // 7
};

FournitureItem fournitureItems[] = {
    {"Anvil", 28}, // 0
    {"Chest", 29}, // 1
    {"Oven", 30}, // 2
    {"Furnace", 31}, // 3
    {"Workbench", 32}, // 4
    {"Lantern", 33}, // 5
    {"Loom", 34}, // 6
    {"Enchanter", 35} // 7
};

GenericItem genericItems[] = {
    // index 0 is reserved for NULL item
    {"", 0}, // 0
    {"Flower", 7}, // 1
    {"Wood", 8}, // 2
    {"Stone", 9}, // 3
    {"Sand", 10}, // 4
    {"Dirt", 11}, // 5
    {"Cloud", 12}, // 6
    {"Acorn", 13}, // 7
    {"Cactus", 14}, // 8
    {"Seeds", 15}, // 9
    {"Wheat", 16}, // 10
    {"Slime", 20}, // 11
    {"Coal", 21}, // 12
    {"Iron Ore", 22}, // 13
    {"Gold Ore", 23}, // 14
    {"Iron Ingot", 24}, // 15
    {"Gold Ingot", 25}, // 16
    {"Glass", 26}, // 17
    {"Gem", 27}, // 18
    {"Wood Wall", 51}, // 19
    {"Stone Wall", 52}, // 20
    {"Iron Wall", 53}, // 21
    {"Gold Wall", 54}, // 22
    {"Gem Wall", 55}, // 23
    {"Wool", 56}, // 24
    {"String", 57}, // 25
    {"Leather", 62}, // 26
    {"Wood Arrow", 63}, // 27
    {"Stone Arrow", 64}, // 28
    {"Iron Arrow", 65}, // 29
    {"Gold Arrow", 66}, // 30
    {"Gem Arrow", 67}, // 31
    {"Bone", 68}, // 32
    {"Dungeonkey", 69}, // 33
    {"Wizard Summon", 70}, // 34
    {"Dragon Egg", 71}, // 35
    {"Dragon Scale", 72}, // 36
    {"Bookshelves", 73}, // 37
    {"Magic Dust", 74}, // 38
    {"Coin", 75}, // 39
    {"Scroll Undying", 1001}, // 40
    {"Scroll Regeneration", 1002}, // 41
    {"Scroll Speed", 1003}, // 42
    {"Scroll Strength", 1004}, // 43
    {"Scroll Shielding", 1005}, // 44
    {"Scroll Night Vision", 1006} // 45
};

const unsigned int toolItemCount = sizeof(toolItems) / sizeof(toolItems[0]);
const unsigned int foodItemCount = sizeof(foodItems) / sizeof(foodItems[0]);
const unsigned int fournitureItemCount = sizeof(fournitureItems) / sizeof(fournitureItems[0]);
const unsigned int genericItemCount = sizeof(genericItems) / sizeof(genericItems[0]);
