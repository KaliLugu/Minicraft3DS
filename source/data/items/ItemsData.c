#include "ItemsData.h"
#include "ItemsTypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ItemId getIdFromName(const char *name) {
    if (g_itemTable == NULL || g_itemCount == 0) return 0;
    if (name == NULL) return g_itemTable[0].id;
    for (unsigned int i = 0; i < g_itemCount; ++i) {
        if (strcmp(g_itemTable[i].name, name) == 0) {
            return g_itemTable[i].id;
        }
    }
    return g_itemTable[0].id;
}

const char* getNameFromId(ItemId id) {
    if (id >= g_itemCount) return "NULL";
    return g_itemTable[id].name;
}

unsigned int getToolCountLevel(ItemId id) {
    if (id >= g_itemCount) return 0;
    if (g_itemTable[id].category != ITEM_CAT_TOOL) return 0; // not tool
    return g_itemTable[id].data.tool.countLevel;
}

unsigned int getFoodHealth(ItemId id) {
    if (id >= g_itemCount) return 0;
    if (g_itemTable[id].category != ITEM_CAT_FOOD) return 0; // not food
    return g_itemTable[id].data.food.health;
}

unsigned int getSpellDuration(ItemId id) {
    if (id >= g_itemCount) return 0;
    if (g_itemTable[id].category != ITEM_CAT_SPELL) return 0; // not spell
    return g_itemTable[id].data.spell.duration;
}

unsigned int getSpellEffect(ItemId id) {
    if (id >= g_itemCount) return 0;
    if (g_itemTable[id].category != ITEM_CAT_SPELL) return 0; // not spell
    return g_itemTable[id].data.spell.effect;
}

unsigned int getItemCategory(ItemId id) {
    if (id >= g_itemCount) return 0;
    return g_itemTable[id].category;
}

char *itemGetName(ItemId id, int countLevel) {
    if (id >= g_itemCount) return "";
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

char *itemGetNameWithCount(ItemId id, int countLevel) {
    static char _itemCurrentName[32];
    if (itemIsSingle(id, countLevel)) {
        return itemGetName(id, countLevel);
    } else {
        snprintf(_itemCurrentName, sizeof(_itemCurrentName), "%d %s", countLevel, itemGetName(id, countLevel));
        return _itemCurrentName;
    }
}

int itemGetIconX(ItemId id, int countLevel) {
    if (id >= g_itemCount) return 0;
    if (id == getIdFromName("TOOL_SHOVEL") || 
        id == getIdFromName("TOOL_HOE") ||
        id == getIdFromName("TOOL_SWORD") ||
        id == getIdFromName("TOOL_PICKAXE") ||
        id == getIdFromName("TOOL_AXE") ||
        id == getIdFromName("TOOL_BUCKET")) {
        // handle special cases here
        return g_itemTable[id].texX + countLevel;
    } else return g_itemTable[id].texX;
}

int itemGetIconY(ItemId id, int countLevel) {
    if (id >= g_itemCount) return 0;
    return g_itemTable[id].texY;
}

bool itemIsSingle(ItemId id, int countLevel) {
    if (id >= g_itemCount) return true;
    return !g_itemTable[id].isStackable;
}
