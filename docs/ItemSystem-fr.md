# Système d'Item

## Architecture actuelle

### Définition des items
Les items vanilla sont maintenant définis dans `source/data/items/ItemsTypes.c` via un tableau statique interne `_vanillaDefs[]`.
Ce tableau contient uniquement les définitions des items du jeu de base et n'est pas exposé directement aux autres modules.

La table d'items accessible globalement est :
- `ItemData *g_itemTable` : pointeur vers la table active
- `unsigned int g_itemCount` : taille actuelle de la table

Ces deux symboles sont initialisés par `itemsTableBuild(uint16_t modCount)`.

### Construction de la table
Le chargement d'items se fait en deux phases :
1. `itemsTableBuild(0)` alloue `g_itemTable` sur le heap et copie les définitions vanilla de `_vanillaDefs[]`
2. `itemsDataInit()` initialise les structures dérivées (`_itemIconX`, `_itemIconY`) selon la taille de `g_itemTable`

L'appel est effectué dans `source/Data.c` :
```c
void initData() {
    uint16_t modCount = 0;
    // TODO: scanner sdmc:/mods/ ici
    itemsTableBuild(modCount);
    itemsDataInit();
    tilesDataInit();
    effectsDataInit();
    entityDataInit();
}
```

### Structure des items
Chaque item est décrit par `ItemData` :
- `id` : identifiant runtime unique, assigné séquentiellement lors du build
- `name` : identifiant logique stable (ex: `"TOOL_SWORD"`)
- `displayName` : nom à afficher à l'écran
- `category` : type d'item (`ITEM_CAT_TOOL`, `ITEM_CAT_FOOD`, `ITEM_CAT_FURNITURE`, `ITEM_CAT_GENERIC`, `ITEM_CAT_SPELL`)
- `isStackable` : si l'item peut s'empiler
- `texX`, `texY` : coordonnées de la texture dans le spritesheet
- `data` : union contenant les données spécifiques à la catégorie

### Lookups
- `getIdFromName(const char *name)` → ID runtime
- `getNameFromId(ItemId id)` → nom logique

### Gestion dynamique des données dérivées
Les tableaux d'affichage sont désormais heap-alloués :
- `_itemIconX`
- `_itemIconY`

Ils sont créés dans `itemsDataInit()` avec `calloc(g_itemCount, ...)`, ce qui permet de supporter un nombre d'items variable.

> `_itemNames` et `_itemSingle` ont été supprimés — ces données sont désormais directement dans `ItemData` (`displayName` et `isStackable`).

### Sécurité des bornes
Les vérifications utilisent maintenant `g_itemCount` au lieu d'une constante fixe.
Cela permet d'étendre la table à des items moddés tout en conservant des vérifications valides.

### Sérialisation (saves)
Le système de sauvegarde reste basé sur des noms stables, ce qui protège les parties contre un changement d'ordre dans la table d'items.
La compatibilité est maintenue tant que les noms vanilla restent inchangés.

---

## Ce qui a changé

- `ItemsTypes.c`
  - `_vanillaDefs[]` contient les définitions vanilla
  - `itemsTableBuild(uint16_t modCount)` alloue `g_itemTable` dynamiquement
  - `g_itemCount` est défini en runtime
  - `ItemsTables[]` n'existe plus
- `ItemsTypes.h`
  - plus d'`extern ItemData ItemsTables[]`
  - plus d'`extern const unsigned int itemCount`
  - exporte `g_itemTable`, `g_itemCount`, `itemsTableBuild()`
- `ItemsData.c`
  - les arrays d'affichage (`_itemIconX`, `_itemIconY`) sont heap-alloués via `calloc`
  - `_itemNames` et `_itemSingle` supprimés (données migrées dans `ItemData`)
  - `_itemRegister()` : paramètre `isSingle` supprimé (signature : `_itemRegister(id, name, iconX, iconY)`)
  - `getNameFromId()` et les bornes utilisent `g_itemCount`
- `Data.c`
  - appelle `itemsTableBuild(0)` avant `itemsDataInit()`

---

## Vision modding

L'architecture actuelle est prête pour le prochain palier :
- charger d'abord les items vanilla
- scanner un dossier `mods/`
- allouer `g_itemTable` avec `modCount` items supplémentaires
- copier les items moddés après les items vanilla
- conserver des IDs séquentiels et des noms stables

### Avantages
- la table d'items est maintenant extensible à runtime
- la logique de recherche reste la même (`getIdFromName`, `getNameFromId`)
- les structures liées à l'affichage des items ne sont plus rigides

---

## Fichiers clés

| Fichier | Rôle |
|---|---|
| `source/data/items/ItemsData.h` | Déclaration de `ItemData`, `ItemId`, `ItemCategory`, fonctions liées aux items |
| `source/data/items/ItemsTypes.h` | Expose `g_itemTable`, `g_itemCount` et `itemsTableBuild()` |
| `source/data/ItemsTypes.c` | Définitions vanilla, build dynamique de la table d'items |
| `source/data/ItemsData.c` | Recherche d'items, initialisation des arrays dérivés, bounds checks |
| `source/Data.c` | Ordre d'initialisation : build de la table puis init des données |
| `source/SaveLoad.c` | Sérialisation et désérialisation des items via noms logiques |
