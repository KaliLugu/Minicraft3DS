#pragma once
#include "Item.h"
#include <stdarg.h>

typedef struct _recipecost {
    int costItem;
    int costAmount;
} Cost;

typedef struct _recipe {
    bool canCraft;
    int itemResult;
    int itemAmountLevel;
    int numOfCosts;
    Cost costs[6]; // Up to 6 items for costs
    int order;     // Used for stable sorting.
} Recipe;

typedef struct _recipeManager {
    int size;
    Recipe *recipes;
} RecipeManager;

extern RecipeManager workbenchRecipes;
extern RecipeManager furnaceRecipes;
extern RecipeManager ovenRecipes;
extern RecipeManager anvilRecipes;
extern RecipeManager loomRecipes;
extern RecipeManager enchanterRecipes;

extern Recipe defineRecipe(int item, int amountOrLevel, int numArgs, ...);

extern void cloneRecipeManager(RecipeManager *from, RecipeManager *to);
extern void checkCanCraftRecipes(RecipeManager *rm, Inventory *inv);
extern void sortRecipes(RecipeManager *rm);
extern bool craftItem(RecipeManager *rm, Recipe *r, Inventory *inv);

extern void initRecipes();
extern void freeRecipes();
