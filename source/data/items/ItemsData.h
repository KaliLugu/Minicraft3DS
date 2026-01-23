#pragma once

#include "../../engine/dtypes.h"
#include <stdint.h>

// TODO HERE 
//
// mettre une boucle for pour générer automatiquement les id des items et ne plus devoir les setup a la main
// commencé a écrire une doc sur le futur système d'item dans le doc "docs/ItemSystem.md"

typedef struct { // structure de base pour une catégorie d'item
    const char* base_name; // le nom de base de l'item (ex: "Sword", "Apple", etc)
    const char** level_names; // tableau des noms par niveau (ex: "Wood Sword", "Iron Sword", etc)
    unsigned int level_count; // nombre de niveaux disponibles pour cet item
    int legacy_id; // l'id legacy de l'item pour la compatibilité
} ToolItem;

typedef struct { // structure pour les items commestibles
    const char* base_name; // le nom de base de l'item (ex: "Bread", "Apple", etc)
    int heal_value; // la valeur de soin de l'item
    int legacy_id; // l'id legacy de l'item pour la compatibilité
} FoodItem;

typedef struct { // structure de base pour un item générique
    const char* base_name; // le nom de base de l'item
    int legacy_id; // l'id legacy de l'item pour la compatibilité
} GenericItem;

typedef enum {
    ITEM_CATEGORY_NONE = 0,
    ITEM_CATEGORY_TOOL,
    ITEM_CATEGORY_FOOD,
    ITEM_CATEGORY_GENERIC
} ItemCategory;


typedef struct { // structure pour identifier un item
    ItemCategory category;  // 0 = NULL
    uint16_t id;       // index dans la catégorie
} ItemID;

extern void itemsDataInit();

extern char *itemGetName(int id, int countLevel);
extern char *itemGetNameWithCount(int id, int countLevel);
extern int itemGetIconX(int id, int countLevel);
extern int itemGetIconY(int id, int countLevel);
extern bool itemIsSingle(int id, int countLevel);
extern unsigned int toolItemCount;
extern ToolItem toolItems[];
extern FoodItem foodItems[];
extern GenericItem genericItems[];
extern const unsigned int toolItemCount;
extern const unsigned int foodItemCount;
extern const unsigned int genericItemCount;