5.1# Système de Rendu des Menus - Minicraft 3DS

## Table des matières

1. [Vue d'ensemble](#vue-densemble)
2. [Architecture générale](#architecture-générale)
3. [Écrans et résolutions](#écrans-et-résolutions)
4. [Fonctions de rendu principales](#fonctions-de-rendu-principales)
5. [Système de texte](#système-de-texte)
6. [Rendu texte intelligemment (wrap + centrage)](#rendu-texte-intelligemment-wrap-+-centrage)
7. [Système de tuiles](#système-de-tuiles)
8. [Limitations de l'écran réduit](#limitations-de-lécran-réduit)
9. [Patterns et bonnes pratiques](#patterns-et-bonnes-pratiques)
10. [Exemples pratiques](#exemples-pratiques)

---

## Vue d'ensemble

Le système de rendu des menus de Minicraft 3DS est basé sur un système de double écran typique de la Nintendo 3DS :
- **Écran supérieur** (Top Screen) : 400×240 pixels
- **Écran inférieur** (Bottom Screen) : 320×240 pixels

Chaque menu implémente deux fonctions clés :
- `menuNameTick()` : Gère la logique et les entrées
- `menuNameRender(int screen, int width, int height)` : Affiche le contenu

---

## Architecture générale

### Cycle de rendu

```c
// Dans la fonction principale du jeu
void renderMenu(int menu, int screen, int width, int height) {
    switch (menu) {
    case MENU_TITLE:
        menuTitleRender(screen, width, height);
        break;
    // ... autres menus
    }
}
```

### Structure d'une fonction de rendu

```c
void menuNameRender(int screen, int width, int height) {
    // Écran supérieur (Top Screen) = screen 0
    if (screen == 0) {
        // Rendu pour l'écran supérieur
        drawRect(0, 0, width, height, 0x0C0C0CFF);  // Fond noir
    }
    
    // Écran inférieur (Bottom Screen) = screen 10
    if (screen == 10) {
        // Rendu pour l'écran inférieur
        renderText("Contenu", 10, 10);
    }
}
```

---

## Écrans et résolutions

### Identification des écrans

| Paramètre | Écran | Résolution | Utilisation |
|-----------|-------|------------|------------|
| `screen == 0` | Top (Supérieur) | 400×240 pixels | Titre, menu principal, informations |
| `screen == 10` | Bottom (Inférieur) | 320×240 pixels | Options, contrôles, contenu détaillé |

### Variables de dimensions

- `width` : Largeur actuelle de l'écran (400 ou 320)
- `height` : Hauteur actuelle de l'écran (toujours 240)

**Important** : L'écran inférieur est plus étroit. Les calculs de centrage doivent tenir compte de cette différence !

```c
// Centrer du texte correctement
int centerX = (width / 2 - (strlen(text) * 8)) / 2;
renderText(text, centerX, y);
```

---

## Fonctions de rendu principales

### 1. `drawRect(float x, float y, float w, float h, MColor color)`

Dessine un rectangle rempli.

**Paramètres :**
- `x, y` : Position du coin supérieur gauche
- `w, h` : Largeur et hauteur du rectangle
- `color` : Couleur au format RGBA (0xRRGGBBAA)

**Exemple :**
```c
drawRect(0, 0, width, height, 0x0C0C0CFF);  // Fond noir semi-opaque
drawRect(10, 10, 100, 50, 0xFF0000FF);      // Rectangle rouge
```

**Format des couleurs :**
```
0xRRGGBBAA
- RR : Composante rouge (00-FF)
- GG : Composante verte (00-FF)
- BB : Composante bleue (00-FF)
- AA : Alpha/opacité (FF = opaque, 00 = transparent)

Exemples :
0x000000FF   = Noir opaque
0xFFFFFFFF   = Blanc opaque
0xFF0000FF   = Rouge opaque
0x0C0C0CFF   = Gris très foncé (fond par défaut)
0x7F7F7FFF   = Gris moyen
```

### 2. `renderText(char *msg, int x, int y)`

Affiche du texte simple en blanc.

**Paramètres :**
- `msg` : Chaîne de caractères
- `x, y` : Position du texte

**Caractères supportés :**
```
ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.,!?'-+=/\%()<>:;
```

**Note :** Tous les caractères minuscules sont automatiquement convertis en majuscules.

```c
renderText("HELLO", 10, 20);
renderText("Hello World", 10, 40);  // Affichera "HELLO WORLD"
```

### 3. `renderTextCentered(char *text, int y, int width)`

Affiche du texte centré horizontalement.

**Paramètres :**
- `text` : Chaîne de caractères
- `y` : Position verticale
- `width` : Largeur disponible pour le centrage

**Exemple :**
```c
renderTextCentered("Start Game", 100, width);
// Adapte automatiquement au 400 ou 320 pixels
```

### 4. `renderTextColor(char *msg, int x, int y, MColor color)`

Affiche du texte avec couleur personnalisée.

**Paramètres :**
- `msg` : Chaîne de caractères
- `x, y` : Position
- `color` : Couleur RGBA

**Exemple :**
```c
// Texte blanc standard
renderTextColor("Start Game", 50, 50, 0xFFFFFFFF);

// Texte grisé (option désactivée)
renderTextColor("Multiplayer", 50, 70, 0x2F2F2FFF);

// Texte jaune doré
renderTextColor("Selection", 50, 90, 0xFFFF7FFF);
```

### 5. `renderTextColorSpecial(char *msg, int x, int y, MColor color, MColor color2)`

Affiche du texte avec deux couleurs : l'une avant le premier espace, l'autre après.

**Paramètres :**
- `msg` : Chaîne de caractères (doit contenir au moins un espace)
- `x, y` : Position
- `color` : Couleur avant le premier espace
- `color2` : Couleur après le premier espace

**Exemple :**
```c
// "Hello" en blanc, "World" en jaune
renderTextColorSpecial("Hello World", 50, 50, 0xFFFFFFFF, 0xFFFF7FFF);
```

### 6. `renderFrame(int x1, int y1, int x2, int y2, MColor bgColor)`

Dessine une frame/cadre décoratif avec bordure de tuiles.

**Paramètres :**
- `x1, y1` : Coin supérieur gauche (en unités de 16×16 pixels)
- `x2, y2` : Coin inférieur droit (en unités de 16×16 pixels)
- `bgColor` : Couleur de fond RGBA

**Important :** Les coordonnées sont en **unités de 16 pixels**, pas en pixels directs !

```c
// Frame de 24×14 unités = 384×224 pixels
renderFrame(1, 1, 24, 14, 0xAF1010FF);  // Fond rougeâtre

// Frame de 4×4 unités = 64×64 pixels
renderFrame(11, 3, 19, 6, 0x1010FFFF);  // Fond bleuâtre
```

**Division du frame :**
- Intérieur : Zone remplie de la couleur
- Bordures : Tuiles décoratives (coin, côtés, etc.)

### 7. `renderTitle(int x, int y)`

Affiche le titre "MINICRAFT HOMEBREW EDITION" avec les tiles graphiques.

**Paramètres :**
- `x, y` : Position où afficher le titre

**Exemple :**
```c
renderTitle(76, 12);  // Affiche le titre du jeu
```

### 8. `renderButtonIcon(int keyIcon, int x, int y)`

Affiche les icônes des boutons de contrôle.

**Icônes disponibles :**
```
Stick circulaire :
- CIRCLEPAD

D-Pad (gauche) :
- I_SL_UP, I_SL_DOWN, I_SL_LEFT, I_SL_RIGHT

C-Stick (droite, New 3DS seulement) :
- CSTICK
- I_SR_UP, I_SR_DOWN, I_SR_LEFT, I_SR_RIGHT

Boutons d'action :
- I_A, I_B, I_X, I_Y
- I_DP_UP, I_DP_DOWN, I_DP_LEFT, I_DP_RIGHT
- I_START_PLUS, I_SELECT_MINUS
- I_L, I_R, I_ZL, I_ZR (New 3DS)
```

**Exemple :**
```c
renderButtonIcon(I_A, 50, 50);      // Affiche le bouton A
renderButtonIcon(I_SL_UP, 100, 100); // Affiche la flèche haut du D-Pad
```

### 9. `renderTile16(int xp, int yp, int xTile, int yTile, int bits)`

Affiche une tuile 16×16 pixels depuis la texture.

**Paramètres :**
- `xp, yp` : Position à l'écran (en pixels)
- `xTile, yTile` : Coordonnées de la tuile dans la texture
- `bits` : Paramètres de la tuile (couche, orientation, etc.)

**Exemple :**
```c
renderTile16(64, 112, 8, 10, 0);  // Tuile à position (64,112)
```

### 10. `renderTile8(int xp, int yp, int xTile, int yTile, int bits)`

Affiche une tuile 8×8 pixels depuis la texture.

**Paramètres :**
- `xp, yp` : Position à l'écran
- `xTile, yTile` : Coordonnées de la tuile
- `bits` : Paramètres de la tuile

### 11. `renderTile32/64(...)`

Affichent des tuiles plus grandes (32×32 et 64×64 pixels).

---

## Système de texte

### Police de caractères

La police utilisée est une **police bitmap monospace** avec caractères de 8×8 pixels.

### Rendu texte intelligemment (wrap + centrage)

Pour les messages d’erreur ou longs textes de menu, utilise une fonction partagée qui :
- remplit une ligne mot par mot (word wrap)
- n’autorise pas plus que `maxLineChars` chars par ligne
- centre chaque ligne avec `centerAndRender`

Exemple :
```c
static void centerAndRender(const char *text, int y, int width) {
    int centerX = ((width / 2) - strlen(text) * 8) / 2;
    renderText((char *)text, centerX, y);
}

static void centerRenderWrapped(const char *text, int y, int width, int maxLineChars) {
    char copy[256];
    snprintf(copy, sizeof(copy), "%s", text);
    char *word = strtok(copy, " ");
    char line[256] = "";
    int currentY = y;

    while (word != NULL) {
        int lineLen = strlen(line);
        int wordLen = strlen(word);
        int nextLen = lineLen > 0 ? lineLen + 1 + wordLen : wordLen;
        if (nextLen > maxLineChars) {
            if (lineLen > 0) {
                centerAndRender(line, currentY, width);
                currentY += 20;
                line[0] = '\0';
            }
        }

        if (line[0] != '\0')
            strcat(line, " ");
        strcat(line, word);

        word = strtok(NULL, " ");
    }

    if (line[0] != '\0') {
        centerAndRender(line, currentY, width);
    }
}
```

Utilise cette fonction dans `MenuErrorLoadingRender` :
```c
centerRenderWrapped("Save file version is incompatible with this version of the game.", messageY, width, 20);
```

**Caractères supportés :**
```
ABCDEFGHIJKLMNOPQRSTUVWXYZ (majuscules)
0123456789 (chiffres)
.,!?' (ponctuation)
"-+=/\%()<>:; (symboles mathématiques et opérateurs)
```

**Limitation :** Les caractères accentués français (é, è, ê, à, etc.) ne sont **pas supportés** en rendu direct.

### Calculs de centrage

**Largeur fixe du caractère :** 8 pixels

```c
// Calculer la largeur d'une chaîne
int strlen(text) * 8  // Largeur en pixels

// Centrer horizontalement
int centerX = (width - strlen(text) * 8) / 2;
renderText(text, centerX, y);

// Centrer avec décalage pour effet de bordure
renderTextColor(msg, centerX + 1, y + 1, 0x000000FF);  // Noir (ombre)
renderTextColor(msg, centerX, y, textColor);           // Couleur texte
```

### Limitation unique : `renderTextCentered` bug

⚠️ **ATTENTION :** La fonction `renderTextCentered` a un calcul de centrage incorrect :

```c
void renderTextCentered(char *text, int y, int width) {
    renderText(text, (width / 2 - strlen(text) * 8) / 2, y);
    // Formule problématique !
}
```

**Meilleure approche :**
```c
int centerX = (width - strlen(text) * 8) / 2;
renderText(text, centerX, y);
```

---

## Système de tuiles

### Tuiles et textures

Les tuiles sont des petites images (8×8, 16×16, 32×32 ou 64×64 pixels) stockées dans une texture commune.

**Zones de texture :**
- Tuiles de jeu (terrain, items, etc.)
- Icônes UI (boutons, curseurs)
- Texte (police)
- Éléments du titre

### Tuiles décoratives du frame

La fonction `renderFrame` utilise automatiquement les tuiles 25-30 en ligne Y pour les bordures :

```c
getFrame(x1, x2, startX)  // Détermine le type d'angle/côté
```

---

## Limitations de l'écran réduit

### 1. Différence de résolution

**Problème principal :** L'écran inférieur est **80 pixels plus étroit** (320 vs 400).

```
Top Screen    : 400×240 pixels
Bottom Screen : 320×240 pixels
Différence    : -80 pixels en largeur
```

### 2. Stratégies d'adaptation

#### a) Utiliser la variable `width`

```c
void menuRender(int screen, int width, int height) {
    if (screen == 10) {
        // width = 320 pour l'écran inférieur
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        // Centrer correctement peu importe la largeur
        int centerX = (width - strlen(text) * 8) / 2;
        renderText(text, centerX, 50);
    }
}
```

#### b) Adapter le layout pour l'écran inférieur

```c
void menuRender(int screen, int width, int height) {
    if (screen == 0) {
        // Écran supérieur : beaucoup d'espace
        renderTitle(76, 12);           // Centré à 76 (400/2 - offset)
        
        for (int i = 0; i < 6; i++) {
            renderText(options[i], 100, 50 + i*20);
        }
    }
    
    if (screen == 10) {
        // Écran inférieur : moins d'espace
        // width/2 - strlen(text)*8 / 2 pour centrer dans 320 pixels
        int centerX = (width / 2 - strlen(text) * 8) / 2;
        renderText(text, centerX, 50);
    }
}
```

#### c) Utiliser des calculs relatifs

```c
// ✗ Ne pas faire ça (hardcodé pour 400 pixels)
renderText(text, 150, 50);

// ✓ Faire ça (adaptatif)
renderText(text, width / 4, 50);
```

### 3. Problèmes courants

#### Problème 1 : Texte mal centré sur l'écran inférieur

```c
// ✗ Mauvais : fonctionne pour 400 pixels
int x = 200 - strlen(text) * 4;  // Centre pour 400 seulement

// ✓ Bon : s'adapt automatiquement
int x = (width - strlen(text) * 8) / 2;
```

#### Problème 2 : Débordement du contenu

```c
// ✗ Mauvais : peut dépasser sur l'écran inférieur
drawRect(100, 50, 250, 100, color);  // 100+250 = 350 > 320

// ✓ Bon : vérifier les limites
if (xEnd > width) xEnd = width;
```

#### Problème 3 : Frames mal positionnés

```c
// ✗ Mauvais : suppose 400 pixels
renderFrame(1, 1, 25, 14, color);  // 25 unités = 400 pixels

// ✓ Bon : adapter à la largeur
int maxTiles = width / 16;
renderFrame(1, 1, maxTiles, 14, color);
```

### 4. Valeurs typiques pour les deux écrans

| Élément | Top (400px) | Bottom (320px) |
|---------|------------|---------------|
| Texte centré | x = 200 - len×4 | x = 160 - len×4 |
| 1/4 écran | x = 100 | x = 80 |
| 1/2 écran | x = 200 | x = 160 |
| 3/4 écran | x = 300 | x = 240 |
| Espace bord | 10-20px | 5-15px |

---

## Patterns et bonnes pratiques

### 1. Structure standard d'un menu

```c
#include "MenuName.h"
#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"
#include "../Inputs.h"

static int selectedOption = 0;

void menuNameInit() {
    selectedOption = 0;
}

void menuNameTick() {
    // Gestion des inputs
    if (localInputs.k_up.clicked)
        selectedOption--;
    if (localInputs.k_down.clicked)
        selectedOption++;
    if (localInputs.k_accept.clicked)
        // Faire quelque chose
    if (localInputs.k_decline.clicked)
        currentMenu = MENU_TITLE;
}

void menuNameRender(int screen, int width, int height) {
    if (screen == 0) {
        // Écran supérieur
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTitle(76, 12);
    }
    if (screen == 10) {
        // Écran inférieur
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Title", 20, width);
        
        for (int i = 0; i < numOptions; i++) {
            MColor color = (i == selectedOption) ? 0xFFFFFFFF : 0x7F7F7FFF;
            renderTextColor(options[i], x, y + i*20, color);
        }
    }
}
```

### 2. Gestion de la sélection avec couleurs

```c
MColor color = 0x7F7F7FFF;  // Gris (non-sélectionné)
if (i == selectedOption)
    color = 0xFFFFFFFF;      // Blanc (sélectionné)

renderTextColor(options[i], x, y, color);
```

### 3. Effet d'ombre pour le contraste

```c
// Ombre noire
renderTextColor(msg, x + 1, y + 1, 0x000000FF);
// Texte brillant par-dessus
renderTextColor(msg, x, y, 0xFFFFFFFF);
```

### 4. Éléments entre les deux écrans

```c
// Écran supérieur : menu principal
if (screen == 0) {
    renderTitle(...);
    // Options principales
}

// Écran inférieur : descriptions ou contrôles
if (screen == 10) {
    // Description détaillée
    // Guide des touches
    // Image/icon pour la sélection actuelle
}
```

---

## Exemples pratiques

### Exemple 1 : Menu simple avec options

```c
void menuSimpleRender(int screen, int width, int height) {
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTitle(76, 12);
    }
    
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        char *options[] = {"Option 1", "Option 2", "Option 3"};
        
        for (int i = 0; i < 3; i++) {
            MColor color = (i == selectedOption) ? 0xFFFFFFFF : 0x7F7F7FFF;
            
            // Ombre
            renderTextColor(options[i], 50, 60 + i*30, 0x000000FF);
            // Texte
            renderTextColor(options[i], 49, 59 + i*30, color);
        }
    }
}
```

### Exemple 2 : Menu avec cadre

```c
void menuFrameRender(int screen, int width, int height) {
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        // Cadre de 20×12 unités = 320×192 pixels
        renderFrame(0, 1, 20, 13, 0x1010FFFF);
        
        // Texte dans le cadre
        renderTextCentered("Main Menu", 30, width);
    }
}
```

### Exemple 3 : Menu avec description contextuelle

```c
void menuContextRender(int screen, int width, int height) {
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        // Liste des options
        for (int i = 0; i < numOptions; i++) {
            // ...
        }
    }
    
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("--- Description ---", 20, width);
        
        // Afficher la description basée sur la sélection
        switch (selectedOption) {
        case 0:
            renderTextCentered("Start a new game", 60, width);
            break;
        case 1:
            renderTextCentered("Host multiplayer game", 60, width);
            break;
        // ...
        }
    }
}
```

### Exemple 4 : Adaptation écrans (Top/Bottom)

```c
void menuAdaptiveRender(int screen, int width, int height) {
    if (screen == 0) {
        // Top screen : beaucoup d'espace (400px)
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTitle(76, 12);  // Centré pour 400px
        
        for (int i = 0; i < 6; i++) {
            int x = 100 - strlen(options[i]) * 4;  // Pour 400px
            renderTextColor(options[i], x, 50 + i*15, colors[i]);
        }
    }
    
    if (screen == 10) {
        // Bottom screen : moins d'espace (320px)
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        for (int i = 0; i < 4; i++) {
            // Centrage adaptatif
            int centerX = (width - strlen(options[i]) * 8) / 2;
            renderTextColor(options[i], centerX, 80 + i*30, colors[i]);
        }
    }
}
```

---

## Résumé des points clés

✓ **À faire :**
- Utiliser `width` pour tous les calculs de position
- Adapter le layout entre écrans (top 400px vs bottom 320px)
- Utiliser des calculs relatifs, pas des valeurs hardcodées
- Tester sur l'écran inférieur qui est plus étroit
- Utiliser des ombres noires pour améliorer la lisibilité

✗ **À éviter :**
- Hardcoder les positions (150, 200, etc.)
- Supposer une largeur de 400px partout
- Laisser du texte déborder de l'écran
- Utiliser des caractères accentués en direct dans le rendu texte
- Ignorer le paramètre `width` lors de calculs

---

**Dernière mise à jour :** Avril 2026  
**Version :** 1.0  
**Langue :** Français
