# Documentation Système de Menus Minicraft3DS

## Aperçu

Le système de menus de Minicraft3DS est une architecture modulaire qui gère tous les écrans UI et les interactions du jeu. Il suit une approche basée sur les états où chaque menu est un état distinct géré indépendamment avec ses propres fonctions de tick et de rendu.

## Architecture

### Flux Principal des Menus

```
Boucle Principale (main.c)
    ├─ tickMenu(currentMenu)     // Traiter l'entrée & la logique
    └─ renderMenu(currentMenu)   // Dessiner à l'écran
```

Le système de menus est contrôlé par une variable globale `currentMenu` qui contient l'ID de l'état de menu actif. À chaque frame:
1. **Phase Tick**: Traitement des entrées et mises à jour de la logique
2. **Phase Render**: Affichage visuel aux écrans

### Structure de Définition d'un Menu

Chaque menu suit un modèle cohérent avec deux fichiers:

- **MenuName.h**: Fichier d'en-tête avec les déclarations de fonctions
- **MenuName.c**: Implémentation avec deux fonctions principales

Structure d'exemple:
```c
// MenuName.h
#pragma once
extern void menuNameInit();        // Initialiser l'état du menu
extern void menuNameTick();        // Traiter les entrées & mises à jour
extern void menuNameRender(int screen, int width, int height);  // Dessiner
```

### IDs des Menus

Tous les IDs des menus sont définis dans `Menu.h` avec les macros `#define MENU_*`:

```c
#define MENU_NONE 0
#define MENU_TITLE 1
#define MENU_CHOOSE_GAME 2
#define MENU_LOADGAME 3
#define MENU_ERROR_LOADING 14
```

## Composants des Menus

### Fonction Tick

Appelée une fois par frame avec l'entrée utilisateur disponible. Responsabilités:
- Lire l'entrée de `localInputs` (clavier/manette traité par le système d'entrée)
- Mettre à jour l'état du menu
- Changer `currentMenu` pour basculer vers un différent menu
- Appeler les fonctions d'initialisation du jeu lors de l'entrée dans le jeu

**Structure d'entrées** (`Inputs.h`):
- `localInputs.k_accept.clicked` - Bouton confirmer
- `localInputs.k_decline.clicked` - Bouton annuler/retour
- `localInputs.k_up.clicked` / `k_down.clicked` / `k_left.clicked` / `k_right.clicked`
- `localInputs.k_delete.clicked` - Supprimer/Action spécialisée

### Fonction Render

Appelée une fois par frame pour dessiner les visuels. Paramètres:
- `screen`: Quel écran 3DS dessiner
  - `0`: Écran supérieur
  - `10`: Écran inférieur (valeur d'offset pour le système dual-écran 3DS)
- `width`: Largeur de l'écran en pixels
- `height`: Hauteur de l'écran en pixels

Utilitaires de rendu disponibles:
- `drawRect(x, y, width, height, color)` - Dessiner un rectangle coloré
- `renderTextCentered(text, y, width)` - Texte centré
- `renderText(text, x, y)` - Texte positionné
- `renderTextFitWidth(text, x, y, maxWidth)` - Texte avec largeur max

### Format des Couleurs

Les couleurs sont au format RGBA sous forme d'`unsigned int` 32-bits:
- `0xRRGGBBAA` ou `0x0C0C0CFF` (arrière-plan sombre avec alpha complet)

## Variables d'État Globales

De `Globals.h`:

```c
extern int currentMenu;          // ID du menu actif actuel
extern int currentSelection;     // Élément sélectionné dans les menus de liste
extern char currentFileName[256]; // Nom du fichier de sauvegarde en cours
extern int initGame;             // Signal pour démarrer le jeu solo
extern int initMPGame;           // Signal pour démarrer le jeu multijoueur
```

## Transitions de Menus

Pour basculer vers un autre menu, définissez `currentMenu` sur une valeur valide `MENU_*`:

```c
currentMenu = MENU_TITLE;  // Aller à l'écran titre
currentSelection = 0;       // Réinitialiser la sélection
```

## Intégration du Système de Fichiers

### Chargement/Sauvegarde

Le jeu stocke les sauvegardes dans des fichiers `.msv` (Minicraft Save) compressés avec ZIP. Fonctions associées dans `SaveLoad.h`:

```c
bool loadWorld(filename, eManager, worldData, players, playerCount);
bool saveWorld(filename, eManager, worldData, players, playerCount);
void getWorldPlayerState(filename, score, win);
```

### Codes d'Erreur

Les opérations de chargement retournent des codes d'erreur pour aider au débogage:

```c
#define LOAD_ERROR_NONE 0              // Succès
#define LOAD_ERROR_VERSION_MISMATCH 1  // Sauvegarde d'une version différente
#define LOAD_ERROR_LEGACY_SAVE 2       // Ancien format de sauvegarde
#define LOAD_ERROR_ZIP_FAILED 3        // Fichier ZIP corrompu
#define LOAD_ERROR_MISSING_WORLD 4     // Données du monde manquantes
```

Récupérés via: `int errorCode = getLastLoadError();`

## Menu de Gestion d'Erreur

### MenuErrorLoading

Affiché quand le chargement d'une sauvegarde rencontre des problèmes. Fournit:
- Messages d'erreur spécifiques expliquant le problème
- Deux options: "Continuer" (essayer avec données partielles) ou "Retour au menu"

**Fonctions**:
```c
void MenuErrorLoadingInit(int errorCode);     // Définir le code d'erreur
void MenuErrorLoadingTick();                  // Gérer l'entrée
void MenuErrorLoadingRender(int screen, int width, int height);  // Afficher
```

**Messages d'Erreur**:
- **Mismatch de Version**: "La version de la sauvegarde est incompatible avec cette version du jeu."
- **Sauvegarde Ancienne**: "C'est un ancien format de sauvegarde. Certaines données peuvent ne pas être chargées correctement."
- **ZIP Échoué**: "Le fichier de sauvegarde est corrompu ou ne peut pas être lu."
- **Monde Manquant**: "Les données du monde manquent dans le fichier de sauvegarde."

**Flux d'Interaction Utilisateur**:
1. Le joueur essaie de charger une sauvegarde
2. `loadWorld()` échoue et retourne à `startGame()` dans `Ingame.c`
3. Le code d'erreur est récupéré
4. `MenuErrorLoadingInit()` est appelé avec le code d'erreur
5. `currentMenu` est défini sur `MENU_ERROR_LOADING`
6. L'utilisateur voit l'écran d'erreur avec les options
7. **Continuer**: Le jeu procède avec les données chargées (si disponibles)
8. **Retour**: Retour à `MENU_CHOOSE_GAME`

## Créer un Nouveau Menu

### Étape 1: Définir l'ID du Menu

Ajouter à `Menu.h`:
```c
#define MENU_CUSTOM 20
```

### Étape 2: Créer le Fichier d'En-tête

Créer `source/menu/MenuCustom.h`:
```c
#pragma once

extern void menuCustomInit();
extern void menuCustomTick();
extern void menuCustomRender(int screen, int width, int height);
```

### Étape 3: Créer l'Implémentation

Créer `source/menu/MenuCustom.c`:
```c
#include "MenuCustom.h"
#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"
#include "../Inputs.h"

static int selectedOption = 0;

void menuCustomInit() {
    selectedOption = 0;
}

void menuCustomTick() {
    if (localInputs.k_accept.clicked) {
        // Gérer l'acceptation
        currentMenu = MENU_TITLE;
    }
    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
    }
}

void menuCustomRender(int screen, int width, int height) {
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Menu Personnalisé", 20, width);
    }
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Contenu de l'écran inférieur", 50, width);
    }
}
```

### Étape 4: Enregistrer le Menu

Ajouter l'include à `Menu.h`:
```c
#include "menu/MenuCustom.h"
```

Ajouter les cas à `Menu.c`:
```c
// Dans tickMenu()
case MENU_CUSTOM:
    menuCustomTick();
    break;

// Dans renderMenu()
case MENU_CUSTOM:
    menuCustomRender(screen, width, height);
    break;
```

## Coordonnées des Écrans

La 3DS a deux écrans:
- **Écran Supérieur**: 400x240 pixels (ID écran: 0)
- **Écran Inférieur**: 320x240 pixels (ID écran: 10)

Dans les fonctions de rendu, vérifiez le paramètre `screen` pour déterminer lequel dessiner:
```c
if (screen == 0) {
    // Dessin sur écran supérieur
}
if (screen == 10) {
    // Dessin sur écran inférieur
}
```

## Patterns Courants

### Menu Sélection de Liste

```c
static int selectedIndex = 0;
static int itemCount = 5;

void tick() {
    if (localInputs.k_up.clicked) {
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = itemCount - 1;
    }
    if (localInputs.k_down.clicked) {
        selectedIndex++;
        if (selectedIndex >= itemCount) selectedIndex = 0;
    }
    if (localInputs.k_accept.clicked) {
        // Gérer la sélection
    }
}

void render(int screen, int width, int height) {
    if (screen == 10) {
        for (int i = 0; i < itemCount; i++) {
            int y = 30 + (i * 30);
            int color = (i == selectedIndex) ? 0xFFFFFFFF : 0x808080FF;
            // Dessiner l'élément...
        }
    }
}
```

### Affichage d'État Haut/Bas

```c
void menuCustomRender(int screen, int width, int height) {
    if (screen == 0) {
        // Afficher l'état, le titre ou l'arrière-plan en haut
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Titre", 10, width);
    }
    if (screen == 10) {
        // Afficher l'UI principale, les options ou le contenu en bas
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Options", 50, width);
    }
}
```

## Intégration Sauvegarde/Chargement

Lors de l'implémentation d'un menu de chargement:

```c
void menuCustomTick() {
    if (loadGameRequested) {
        // Essayer de charger
        EntityManager eManager = {0};
        WorldData worldData = {0};
        
        if (!loadWorld(filename, &eManager, &worldData, players, playerCount)) {
            // Chargement échoué - afficher le menu d'erreur
            int errorCode = getLastLoadError();
            MenuErrorLoadingInit(errorCode);
            currentMenu = MENU_ERROR_LOADING;
            return;
        }
        
        // Chargement réussi - démarrer le jeu
        currentMenu = MENU_NONE;
        // La logique du jeu gère le rendu en jeu
    }
}
```

## Débogage

Pour ajouter une sortie de débogage, utilisez les fonctions de `Render.h`:
```c
debugMessage("État du menu: %d", currentMenu);
debug("Information personnalisée");
```

## Notes de Performance

- Les menus tournent à 60 FPS max sur 3DS
- Les appels de rendu se produisent pour les deux écrans à chaque frame
- Gardez la logique de la fonction tick minimale (calculs lourds avant le changement de menu)
- Le rendu d'arrière-plan utilise les fonctions `menuRenderMapBG*()` pour un style visuel cohérent

## Détails des Entrées

Noms d'entrées complets disponibles dans `Inputs.h`:
- `localInputs.circlepad_x` / `circlepad_y` - Position du stick analogique
- `localInputs.cstick_x` / `cstick_y` - Position du C-stick (New 3DS)
- Tous les boutons directionnels (haut, bas, gauche, droite)
- Boutons d'action (accepter, décliner)
- Boutons d'épaule (l, r)
- Boutons spécialisés (supprimer, etc.)

Vérifiez `.pressed` vs `.clicked` pour la distinction entre maintenu vs appuyé-ce-frame.
