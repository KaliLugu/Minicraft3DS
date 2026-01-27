#include "Crafting.h"
#include "data/items/ItemsData.h"
#include "data/items/ItemsTypes.h"

#include "Data.h"
#include <string.h>

RecipeManager workbenchRecipes;
RecipeManager furnaceRecipes;
RecipeManager ovenRecipes;
RecipeManager anvilRecipes;
RecipeManager loomRecipes;
RecipeManager enchanterRecipes;

void cloneRecipeManager(RecipeManager *from, RecipeManager *to) {
    // free old manager recipes
    free(to->recipes);

    // copy over recipes
    to->size = from->size;
    to->recipes = (Recipe *)malloc(sizeof(Recipe) * to->size);
    memcpy(to->recipes, from->recipes, sizeof(Recipe) * to->size);
}

void checkCanCraftRecipes(RecipeManager *rm, Inventory *inv) {
    int i, j;
    for (i = 0; i < rm->size; i++) {
        rm->recipes[i].canCraft = true;
        for (j = 0; j < rm->recipes[i].numOfCosts; j++) {
            if (countItemInv(rm->recipes[i].costs[j].costItem, 0, inv) < rm->recipes[i].costs[j].costAmount) {
                rm->recipes[i].canCraft = false;
            }
        }
    }
}

int compareCanCraft(const void *ra, const void *rb) {
    Recipe *r1 = (Recipe *)ra;
    Recipe *r2 = (Recipe *)rb;
    if (r1->canCraft && !r2->canCraft)
        return -1;
    if (!r1->canCraft && r2->canCraft)
        return 1;
    return r1->order - r2->order; // Needed for stable sorting.
}

void sortRecipes(RecipeManager *rm) {
    qsort(rm->recipes, rm->size, sizeof(Recipe), compareCanCraft);
}

void deductCost(Cost c, Inventory *inv) {
    Item *item = getItemFromInventory(c.costItem, inv);
    if (!itemIsSingle(item->id, item->countLevel)) {
        item->countLevel -= c.costAmount;
        if (item->countLevel < 1)
            removeItemFromInventory(item->slotNum, inv);
    } else {
        removeItemFromInventory(item->slotNum, inv);
    }
}

bool craftItem(RecipeManager *rm, Recipe *r, Inventory *inv) {
    if (r->canCraft) {
        int i;
        for (i = 0; i < r->numOfCosts; ++i)
            deductCost(r->costs[i], inv);
        Item item = newItem(r->itemResult, r->itemAmountLevel);

        if (!itemIsSingle(item.id, item.countLevel) && countItemInv(item.id, item.countLevel, inv) > 0) {
            getItemFromInventory(item.id, inv)->countLevel += r->itemAmountLevel;
        } else {
            pushItemToInventoryFront(item, inv);
        }
        checkCanCraftRecipes(rm, inv);
        sortRecipes(rm);
        return true;
    }
    return false;
}

Cost newCost(int i, int c) {
    Cost nc;
    nc.costItem = i;
    nc.costAmount = c;
    return nc;
}

uByte curPlace = 0;
Recipe defineRecipe(int item, int amountOrLevel, int numArgs, ...) {
    Recipe r;
    r.itemResult = item;
    r.itemAmountLevel = amountOrLevel;
    r.numOfCosts = numArgs;
    int i;
    va_list al;
    numArgs <<= 1; // Did this to get rid of a warning.
    va_start(al, numArgs);
    for (i = 0; i < r.numOfCosts; ++i)
        r.costs[i] = newCost(va_arg(al, int), va_arg(al, int));
    va_end(al);
    r.order = curPlace;
    curPlace++;
    return r;
}

void initRecipes() {
    curPlace = 0;
    workbenchRecipes.size = 22;
    workbenchRecipes.recipes = (Recipe *)malloc(sizeof(Recipe) * (workbenchRecipes.size));
    workbenchRecipes.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 4}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 20); // workbench use 20 wood
    workbenchRecipes.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 3}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 20);
    workbenchRecipes.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 2}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 20);
    workbenchRecipes.recipes[3] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 1}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 20);
    workbenchRecipes.recipes[4] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 0}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 5);
    workbenchRecipes.recipes[5] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 5}), 1, 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 11}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 17}), 4);
    workbenchRecipes.recipes[6] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 7}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 24}), 5);
    workbenchRecipes.recipes[7] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), 0, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5);
    workbenchRecipes.recipes[8] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), 0, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5);
    workbenchRecipes.recipes[9] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), 0, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5);
    workbenchRecipes.recipes[10] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), 0, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5);
    workbenchRecipes.recipes[11] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), 0, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5);
    workbenchRecipes.recipes[12] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 7}), 0, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1);
    workbenchRecipes.recipes[13] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 27}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1);
    workbenchRecipes.recipes[14] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 5);
    workbenchRecipes.recipes[15] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 5);
    workbenchRecipes.recipes[16] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 5);
    workbenchRecipes.recipes[17] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 5);
    workbenchRecipes.recipes[18] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 5);
    workbenchRecipes.recipes[19] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 28}), 1, 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1);
    workbenchRecipes.recipes[20] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 19}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 4);
    workbenchRecipes.recipes[21] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 20}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 3}), 4);

    anvilRecipes.size = 17;
    anvilRecipes.recipes = (Recipe *)malloc(sizeof(Recipe) * (anvilRecipes.size));
    anvilRecipes.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), 2, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 5);
    anvilRecipes.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), 2, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 5);
    anvilRecipes.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), 2, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 5);
    anvilRecipes.recipes[3] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), 2, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 5);
    anvilRecipes.recipes[4] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), 2, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 5);
    anvilRecipes.recipes[5] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 29}), 1, 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1);
    anvilRecipes.recipes[6] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), 3, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 5);
    anvilRecipes.recipes[7] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), 3, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 5);
    anvilRecipes.recipes[8] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), 3, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 5);
    anvilRecipes.recipes[9] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), 3, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 5);
    anvilRecipes.recipes[10] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), 3, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 5);
    anvilRecipes.recipes[11] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 30}), 1, 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1);
    anvilRecipes.recipes[12] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 6}), 0, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 10);
    anvilRecipes.recipes[13] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FURNITURE, 7}), 1, 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 27}), 20);
    anvilRecipes.recipes[14] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 21}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 2);
    anvilRecipes.recipes[15] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 22}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 2);
    anvilRecipes.recipes[16] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 3, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 1);

    furnaceRecipes.size = 3;
    furnaceRecipes.recipes = (Recipe *)malloc(sizeof(Recipe) * (furnaceRecipes.size));
    furnaceRecipes.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 13}), 4, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 12}), 1);
    furnaceRecipes.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 14}), 4, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 12}), 1);
    furnaceRecipes.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 17}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 4}), 4, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 12}), 1);

    ovenRecipes.size = 4;
    ovenRecipes.recipes = (Recipe *)malloc(sizeof(Recipe) * (ovenRecipes.size));
    ovenRecipes.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 1}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 10}), 4);
    ovenRecipes.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 5}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 4}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 12}), 1);
    ovenRecipes.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 7}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 6}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 12}), 1);
    ovenRecipes.recipes[3] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 3}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 2}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 8);

    loomRecipes.size = 1;
    loomRecipes.recipes = (Recipe *)malloc(sizeof(Recipe) * (loomRecipes.size));
    loomRecipes.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 24}), 1);

    enchanterRecipes.size = 7;
    enchanterRecipes.recipes = (Recipe *)malloc(sizeof(Recipe) * (enchanterRecipes.size));
    enchanterRecipes.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 2}), 4, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 50);
    enchanterRecipes.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 4}), 4, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 50);
    enchanterRecipes.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 1}), 4, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 50);
    enchanterRecipes.recipes[3] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 3}), 4, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 50);
    enchanterRecipes.recipes[4] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 0}), 4, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 5, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 50);
    enchanterRecipes.recipes[5] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 31}), 1, 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 2}), 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 3, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 25}), 1);
    enchanterRecipes.recipes[6] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 23}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 10);
}

/* Free up allocated memory */
void freeRecipes() {
    free(workbenchRecipes.recipes);
    free(ovenRecipes.recipes);
    free(furnaceRecipes.recipes);
    free(anvilRecipes.recipes);
    free(loomRecipes.recipes);
    free(enchanterRecipes.recipes);
}
