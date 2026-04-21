#pragma once
#include "../../engine/dtypes.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEM_ID 65535 // max id can be use on 16bits

typedef uint16_t ItemId; // ID opaque


typedef enum {
    ITEM_CAT_GENERIC,
    ITEM_CAT_TOOL,
    ITEM_CAT_FOOD,
    ITEM_CAT_FURNITURE,
    ITEM_CAT_SPELL,
} ItemCategory;

typedef struct {
    ItemId id;            // identifiant runtime unique
    const char* name;     // nom logique / stable
    const char* displayName; // nom affiché (optionnel, ou variante)
    uint8_t category;     // TOOL, FOOD, FURNITURE, GENERIC
    bool isStackable;     // si l’item est stackabmes
    uint8_t texX, texY;         // coordonnées X et Y de l'item
    union { // selon la catégorie
        struct {
            sShort countLevel; // les matériaux d'item
        } tool;
        struct {
            uint8_t health; // vie que redonne la bouf
        } food;
        struct {
            uint8_t duration; // durée en seconde
            uint8_t effect; // id de l'effet 
        } spell;
        struct {
            bool destroyAfterUse;
        } generic;
    } data;
} ItemData;

extern char *itemGetName(ItemId id, int countLevel);
extern char *itemGetNameWithCount(ItemId id, int countLevel);
extern int itemGetIconX(ItemId id, int countLevel);
extern int itemGetIconY(ItemId id, int countLevel);
extern bool itemIsSingle(ItemId id, int countLevel);
extern ItemId getIdFromName(const char *name);
extern const char* getNameFromId(ItemId id);
extern unsigned int getToolCountLevel(ItemId id);
extern unsigned int getFoodHealth(ItemId id);
extern unsigned int getSpellDuration(ItemId id);
extern unsigned int getSpellEffect(ItemId id);
