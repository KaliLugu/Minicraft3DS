#include "ItemsData.h"

ToolItem toolItems[] = {
    // index 0 is reserved for NULL item
    {"Shovel", (const char*[]){"Wood Shovel", "Stone Shovel", "Gold Shovel", "Gem Shovel"}, 4, 1}, // 1
    {"Hoe", (const char*[]){"Wood Hoe", "Stone Hoe", "Gold Hoe", "Gem Hoe"}, 4, 2}, // 2
    {"Sword", (const char*[]){"Wood Sword", "Stone Sword", "Gold Sword", "Gem Sword"}, 4, 3}, // 3
    {"Pickaxe", (const char*[]){"Wood Pickaxe", "Stone Pickaxe", "Gold Pickaxe", "Gem Pickaxe"}, 4, 4}, // 4
    {"Axe", (const char*[]){"Wood Axe", "Stone Axe", "Gold Axe", "Gem Axe"}, 4, 5}, // 5
};

FoodItem foodItems[] = {
    {"Flesh", 5, 17}, // 1
    {"Bread", 10, 18}, // 2
    {"Apple", 7, 19}, // 3
    {"Golden Apple", 20, 1007} // 4
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
    {"Golden Apple", 1007}, // 19
    {"Bucket", 101}, // 20
    {"Bow", 102}, // 21
    {"Magic Compass", 103} // 22
};
