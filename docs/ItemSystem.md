# Système d'Item

## Architecture actuelle

### Définition des items
Les items sont définis statiquement dans `source/data/items/ItemsTypes.c` via le tableau `ItemsTables[]`.

Chaque item est décrit par une struct `ItemData` :
- `id` : identifiant numérique runtime (assigné séquentiellement)
- `name` : identifiant stable en string (ex: `"TOOL_SWORD"`) — utilisé pour la sérialisation
- `displayName` : nom affiché à l'écran
- `category` : type d'item (`TOOL`, `FOOD`, `FURNITURE`, `GENERIC`, `SPELL`)
- `isStackable` : si l'item peut s'empiler
- `data` : union de données spécifiques à la catégorie

### Lookups
- `getIdFromName(name)` → ID numérique runtime
- `getNameFromId(id)` → nom stable

### Sérialisation (saves)
Les items sont sauvegardés par leur **nom string** (pas par ID). Cela garantit que les saves restent valides si l'ordre des items change ou si des items sont ajoutés.

---

## Vision modding (data-only)

### Objectif
Permettre l'ajout d'items via des fichiers externes (JSON ou format texte simple) sans recompiler le jeu. Les mods pourront définir des items qui utilisent les comportements existants par catégorie.

### Ce qui est déjà en place
- Saves par nom de string : compatible avec des items chargés dynamiquement
- Struct `ItemData` : correspond directement à ce qu'un fichier externe décrirait
- Registry central `ItemsTables[]` : point d'entrée unique à étendre

### Changements nécessaires pour activer le modding

**1. ID assignment runtime**
Remplacer `__COUNTER__` (résolu à la compilation) par un compteur incrémental au moment de l'enregistrement. Cela permettra d'enregistrer des items depuis un fichier externe avec des IDs valides.

**2. Registry dynamique**
Passer de `ItemsTables[]` (taille fixe) à un tableau alloué dynamiquement (`malloc`/`realloc`). Les items vanilla sont chargés d'abord, puis les items moddés sont ajoutés avant l'init du jeu.

**3. Callbacks comportement (préparation scripting futur)**
Ajouter des pointeurs de fonction optionnels dans `ItemData` pour les comportements custom :
```c
void (*onUse)(Entity* user, Item* item);   // NULL = comportement par défaut de la catégorie
void (*onPickup)(Entity* user, Item* item); // NULL = comportement par défaut
```
Pour les items vanilla, ces pointeurs restent NULL. Cela prépare le scripting sans rien casser.

### Format de fichier cible (data-only)
```json
{
  "name": "MOD_MITHRIL_SWORD",
  "displayName": "Mithril Sword",
  "category": "TOOL",
  "isStackable": false,
  "data": { "countLevel": 3 }
}
```

### Structure des mods
```
mods/
  my_mod/
    items/
      mithril_sword.json
      mithril_pickaxe.json
    textures/     (futur)
```

Chaque mod a son propre dossier. Le nom du dossier identifie le mod (utile pour les logs et la détection de conflits de noms entre mods).

### Pipeline de chargement prévu
1. Chargement des items vanilla (depuis `ItemsTables[]`)
2. Scan du dossier `mods/` — pour chaque sous-dossier de mod, scan de `<mod>/items/*.json`
3. Parse de chaque fichier `.json` et enregistrement dans le registry dynamique
4. Init du jeu (le registry est complet à ce stade)

---

## Fichiers clés

| Fichier | Rôle |
|---|---|
| `source/data/items/ItemsTypes.h` | Struct `ItemData`, enum `ItemCategory` |
| `source/data/items/ItemsTypes.c` | Registry `ItemsTables[]`, définitions vanilla |
| `source/data/items/ItemsData.c` | `getIdFromName()`, `getNameFromId()`, enregistrement icônes |
| `source/Item.h` / `source/Item.c` | Struct runtime `Item`, création et manipulation |
| `source/SaveLoad.c` | Sérialisation/désérialisation par nom |
