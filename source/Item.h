#pragma once
#include "engine/dtypes.h"

typedef struct Inventory Inventory;

typedef struct
{
    sShort id;
    sShort countLevel;   // Count for items, Level for tools.
    sShort slotNum;      // Read-only. Do not mess with this.
    int *invPtr;         // pointer to current inventory.
    Inventory *chestPtr; // pointer to chest inventory for chest item.
} Item;

struct Inventory {
    Item items[300]; // Maximum of 300 slots in every inventory.
    sShort lastSlot; // lastSlot can also be used to read the size of the inventory.
};

extern bool isItemEmpty(Item *item);
extern Item newItem(int id, int cLevel);
extern void pushItemToInventoryFront(Item item, Inventory *inv);
extern void addItemToInventory(Item item, Inventory *inv);
extern void removeItemFromCurrentInv(Item *item);
extern void removeItemFromInventory(int slot, Inventory *inv);
extern Item *getItemFromInventory(int itemID, Inventory *inv);
extern int countItemInv(int itemID, int level, Inventory *inv);
