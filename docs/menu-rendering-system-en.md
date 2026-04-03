# Menu Rendering System - Minicraft 3DS

## Table of Contents

1. [Overview](#overview)
2. [General Architecture](#general-architecture)
3. [Screens and Resolutions](#screens-and-resolutions)
4. [Main Rendering Functions](#main-rendering-functions)
5. [Text System](#text-system)
6. [Fonctions de rendu principales](#fonctions-de-rendu-principales)
5. [Système de texte](#système-de-texte)
6. [Smart Text Rendering (wrap + center)](#smart-text-rendering-wrap-+-center)
7. [Tile System](#tile-system)
8. [Small Screen Limitations](#small-screen-limitations)
9. [Patterns and Best Practices](#patterns-and-best-practices)
10. [Practical Examples](#practical-examples)

---

## Overview

The Minicraft 3DS menu rendering system is based on the dual-screen architecture typical of Nintendo 3DS:
- **Top Screen** : 400×240 pixels
- **Bottom Screen** : 320×240 pixels

Each menu implements two key functions:
- `menuNameTick()` : Handles logic and input
- `menuNameRender(int screen, int width, int height)` : Renders content

---

## General Architecture

### Render Cycle

```c
// In the main game loop
void renderMenu(int menu, int screen, int width, int height) {
    switch (menu) {
    case MENU_TITLE:
        menuTitleRender(screen, width, height);
        break;
    // ... other menus
    }
}
```

### Render Function Structure

```c
void menuNameRender(int screen, int width, int height) {
    // Top Screen = screen 0
    if (screen == 0) {
        // Top screen rendering
        drawRect(0, 0, width, height, 0x0C0C0CFF);  // Black background
    }
    
    // Bottom Screen = screen 10
    if (screen == 10) {
        // Bottom screen rendering
        renderText("Content", 10, 10);
    }
}
```

---

## Screens and Resolutions

### Screen Identification

| Parameter | Screen | Resolution | Usage |
|-----------|--------|------------|-------|
| `screen == 0` | Top | 400×240 pixels | Title, main menu, information |
| `screen == 10` | Bottom | 320×240 pixels | Options, controls, detail content |

### Dimension Variables

- `width` : Current screen width (400 or 320)
- `height` : Current screen height (always 240)

**Important** : The bottom screen is narrower. Centering calculations must account for this difference!

```c
// Center text correctly
int centerX = (width / 2 - (strlen(text) * 8)) / 2;
renderText(text, centerX, y);
```

### Smart Text Rendering (wrap + center)

Une fonction partagée est recommandée pour gérer automatiquement les textes trop longs dans tous les menus (utile pour les erreurs et descriptions). L'objectif :
- découper par mots (word wrap)
- ne pas dépasser un nombre de caractères par ligne (`maxLineChars`)
- centrer chaque ligne avec `centerAndRender`
- réduire les comportements spécifiques coûteux ou duplication

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

Et utilisation dans `MenuErrorLoadingRender` :
```c
centerRenderWrapped("Save file version is incompatible with this version of the game.", messageY, width, 20);
```

---

## Main Rendering Functions

### 1. `drawRect(float x, float y, float w, float h, MColor color)`

Draws a filled rectangle.

**Parameters:**
- `x, y` : Top-left corner position
- `w, h` : Width and height of rectangle
- `color` : Color in RGBA format (0xRRGGBBAA)

**Example:**
```c
drawRect(0, 0, width, height, 0x0C0C0CFF);  // Dark gray background
drawRect(10, 10, 100, 50, 0xFF0000FF);      // Red rectangle
```

**Color Format:**
```
0xRRGGBBAA
- RR : Red component (00-FF)
- GG : Green component (00-FF)
- BB : Blue component (00-FF)
- AA : Alpha/Opacity (FF = opaque, 00 = transparent)

Examples:
0x000000FF   = Black opaque
0xFFFFFFFF   = White opaque
0xFF0000FF   = Red opaque
0x0C0C0CFF   = Very dark gray (default background)
0x7F7F7FFF   = Medium gray
```

### 2. `renderText(char *msg, int x, int y)`

Displays simple white text.

**Parameters:**
- `msg` : Text string
- `x, y` : Text position

**Supported Characters:**
```
ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.,!?'-+=/\%()<>:;
```

**Note:** All lowercase characters are automatically converted to uppercase.

```c
renderText("HELLO", 10, 20);
renderText("Hello World", 10, 40);  // Will display "HELLO WORLD"
```

### 3. `renderTextCentered(char *text, int y, int width)`

Displays horizontally centered text.

**Parameters:**
- `text` : Text string
- `y` : Vertical position
- `width` : Available width for centering

**Example:**
```c
renderTextCentered("Start Game", 100, width);
// Automatically adapts to 400 or 320 pixels
```

### 4. `renderTextColor(char *msg, int x, int y, MColor color)`

Displays text with custom color.

**Parameters:**
- `msg` : Text string
- `x, y` : Position
- `color` : RGBA color

**Example:**
```c
// White standard text
renderTextColor("Start Game", 50, 50, 0xFFFFFFFF);

// Grayed text (disabled option)
renderTextColor("Multiplayer", 50, 70, 0x2F2F2FFF);

// Golden yellow text
renderTextColor("Selection", 50, 90, 0xFFFF7FFF);
```

### 5. `renderTextColorSpecial(char *msg, int x, int y, MColor color, MColor color2)`

Displays text with two colors: one before the first space, another after.

**Parameters:**
- `msg` : Text string (must contain at least one space)
- `x, y` : Position
- `color` : Color before first space
- `color2` : Color after first space

**Example:**
```c
// "Hello" in white, "World" in yellow
renderTextColorSpecial("Hello World", 50, 50, 0xFFFFFFFF, 0xFFFF7FFF);
```

### 6. `renderFrame(int x1, int y1, int x2, int y2, MColor bgColor)`

Draws a decorative frame/box with tile borders.

**Parameters:**
- `x1, y1` : Top-left corner (in 16×16 pixel units)
- `x2, y2` : Bottom-right corner (in 16×16 pixel units)
- `bgColor` : Background color RGBA

**Important:** Coordinates are in **16-pixel units**, not direct pixels!

```c
// Frame of 24×14 units = 384×224 pixels
renderFrame(1, 1, 24, 14, 0xAF1010FF);  // Reddish background

// Frame of 4×4 units = 64×64 pixels
renderFrame(11, 3, 19, 6, 0x1010FFFF);  // Bluish background
```

**Frame Composition:**
- Interior : Area filled with the specified color
- Borders : Decorative tiles (corners, sides, etc.)

### 7. `renderTitle(int x, int y)`

Displays the "MINICRAFT HOMEBREW EDITION" title using graphic tiles.

**Parameters:**
- `x, y` : Position to display title

**Example:**
```c
renderTitle(76, 12);  // Display game title
```

### 8. `renderButtonIcon(int keyIcon, int x, int y)`

Displays control button icons.

**Available Icons:**
```
Circle Pad:
- CIRCLEPAD

D-Pad (left):
- I_SL_UP, I_SL_DOWN, I_SL_LEFT, I_SL_RIGHT

C-Stick (right, New 3DS only):
- CSTICK
- I_SR_UP, I_SR_DOWN, I_SR_LEFT, I_SR_RIGHT

Action Buttons:
- I_A, I_B, I_X, I_Y
- I_DP_UP, I_DP_DOWN, I_DP_LEFT, I_DP_RIGHT
- I_START_PLUS, I_SELECT_MINUS
- I_L, I_R, I_ZL, I_ZR (New 3DS)
```

**Example:**
```c
renderButtonIcon(I_A, 50, 50);      // Display A button
renderButtonIcon(I_SL_UP, 100, 100); // Display D-Pad up arrow
```

### 9. `renderTile16(int xp, int yp, int xTile, int yTile, int bits)`

Displays a 16×16 pixel tile from the texture.

**Parameters:**
- `xp, yp` : Screen position (in pixels)
- `xTile, yTile` : Tile coordinates in texture
- `bits` : Tile parameters (layer, orientation, etc.)

**Example:**
```c
renderTile16(64, 112, 8, 10, 0);  // Tile at position (64,112)
```

### 10. `renderTile8(int xp, int yp, int xTile, int yTile, int bits)`

Displays an 8×8 pixel tile from the texture.

**Parameters:**
- `xp, yp` : Screen position
- `xTile, yTile` : Tile coordinates
- `bits` : Tile parameters

### 11. `renderTile32/64(...)`

Display larger tiles (32×32 and 64×64 pixels).

---

## Text System

### Font

The font used is a **bitmap monospace font** with 8×8 pixel characters.

**Supported Characters:**
```
ABCDEFGHIJKLMNOPQRSTUVWXYZ (uppercase)
0123456789 (digits)
.,!?' (punctuation)
"-+=/\%()<>:; (mathematical symbols and operators)
```

**Limitation:** Accented characters (é, è, ê, à, etc.) are **not supported** in direct rendering.

### Centering Calculations

**Fixed character width:** 8 pixels

```c
// Calculate string width
int strlen(text) * 8  // Width in pixels

// Center horizontally
int centerX = (width - strlen(text) * 8) / 2;
renderText(text, centerX, y);

// Center with offset for border effect
renderTextColor(msg, centerX + 1, y + 1, 0x000000FF);  // Black (shadow)
renderTextColor(msg, centerX, y, textColor);           // Text color
```

### Known Issue: `renderTextCentered` Bug

⚠️ **WARNING:** The `renderTextCentered` function has incorrect centering calculation:

```c
void renderTextCentered(char *text, int y, int width) {
    renderText(text, (width / 2 - strlen(text) * 8) / 2, y);
    // Problematic formula!
}
```

**Better approach:**
```c
int centerX = (width - strlen(text) * 8) / 2;
renderText(text, centerX, y);
```

---

## Tile System

### Tiles and Textures

Tiles are small images (8×8, 16×16, 32×32, or 64×64 pixels) stored in a shared texture.

**Texture Zones:**
- Game tiles (terrain, items, etc.)
- UI icons (buttons, cursors)
- Text (font)
- Title elements

### Frame Decorative Tiles

The `renderFrame` function automatically uses tiles 25-30 in Y-row for borders:

```c
getFrame(x1, x2, startX)  // Determines corner/side type
```

---

## Small Screen Limitations

### 1. Resolution Difference

**Main issue:** Bottom screen is **80 pixels narrower** (320 vs 400).

```
Top Screen    : 400×240 pixels
Bottom Screen : 320×240 pixels
Difference    : -80 pixels width
```

### 2. Adaptation Strategies

#### a) Use the `width` variable

```c
void menuRender(int screen, int width, int height) {
    if (screen == 10) {
        // width = 320 for bottom screen
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        // Center correctly regardless of width
        int centerX = (width - strlen(text) * 8) / 2;
        renderText(text, centerX, 50);
    }
}
```

#### b) Adapt layout per screen

```c
void menuRender(int screen, int width, int height) {
    if (screen == 0) {
        // Top screen: plenty of space
        renderTitle(76, 12);           // Centered at 76 (400/2 - offset)
        
        for (int i = 0; i < 6; i++) {
            renderText(options[i], 100, 50 + i*20);
        }
    }
    
    if (screen == 10) {
        // Bottom screen: less space
        // width/2 - strlen(text)*8 / 2 to center in 320 pixels
        int centerX = (width / 2 - strlen(text) * 8) / 2;
        renderText(text, centerX, 50);
    }
}
```

#### c) Use relative calculations

```c
// ✗ Don't do this (hardcoded for 400 pixels)
renderText(text, 150, 50);

// ✓ Do this (adaptive)
renderText(text, width / 4, 50);
```

### 3. Common Issues

#### Issue 1: Misaligned text on bottom screen

```c
// ✗ Wrong: works only for 400 pixels
int x = 200 - strlen(text) * 4;  // Centers for 400 only

// ✓ Correct: adapts automatically
int x = (width - strlen(text) * 8) / 2;
```

#### Issue 2: Content overflow

```c
// ✗ Wrong: can exceed on bottom screen
drawRect(100, 50, 250, 100, color);  // 100+250 = 350 > 320

// ✓ Correct: check boundaries
if (xEnd > width) xEnd = width;
```

#### Issue 3: Misaligned frames

```c
// ✗ Wrong: assumes 400 pixels
renderFrame(1, 1, 25, 14, color);  // 25 units = 400 pixels

// ✓ Correct: adapt to width
int maxTiles = width / 16;
renderFrame(1, 1, maxTiles, 14, color);
```

### 4. Typical Values for Both Screens

| Element | Top (400px) | Bottom (320px) |
|---------|-----------|--------------|
| Centered text | x = 200 - len×4 | x = 160 - len×4 |
| 1/4 screen | x = 100 | x = 80 |
| 1/2 screen | x = 200 | x = 160 |
| 3/4 screen | x = 300 | x = 240 |
| Edge spacing | 10-20px | 5-15px |

---

## Patterns and Best Practices

### 1. Standard Menu Structure

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
    // Input handling
    if (localInputs.k_up.clicked)
        selectedOption--;
    if (localInputs.k_down.clicked)
        selectedOption++;
    if (localInputs.k_accept.clicked)
        // Do something
    if (localInputs.k_decline.clicked)
        currentMenu = MENU_TITLE;
}

void menuNameRender(int screen, int width, int height) {
    if (screen == 0) {
        // Top screen
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTitle(76, 12);
    }
    if (screen == 10) {
        // Bottom screen
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Title", 20, width);
        
        for (int i = 0; i < numOptions; i++) {
            MColor color = (i == selectedOption) ? 0xFFFFFFFF : 0x7F7F7FFF;
            renderTextColor(options[i], x, y + i*20, color);
        }
    }
}
```

### 2. Selection Coloring

```c
MColor color = 0x7F7F7FFF;  // Gray (unselected)
if (i == selectedOption)
    color = 0xFFFFFFFF;      // White (selected)

renderTextColor(options[i], x, y, color);
```

### 3. Shadow Effect for Contrast

```c
// Black shadow
renderTextColor(msg, x + 1, y + 1, 0x000000FF);
// Bright text on top
renderTextColor(msg, x, y, 0xFFFFFFFF);
```

### 4. Content Between Both Screens

```c
// Top screen: main menu
if (screen == 0) {
    renderTitle(...);
    // Main options
}

// Bottom screen: descriptions or controls
if (screen == 10) {
    // Detailed description
    // Key guide
    // Image/icon for current selection
}
```

---

## Practical Examples

### Example 1: Simple Menu with Options

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
            
            // Shadow
            renderTextColor(options[i], 50, 60 + i*30, 0x000000FF);
            // Text
            renderTextColor(options[i], 49, 59 + i*30, color);
        }
    }
}
```

### Example 2: Menu with Frame

```c
void menuFrameRender(int screen, int width, int height) {
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        // Frame of 20×12 units = 320×192 pixels
        renderFrame(0, 1, 20, 13, 0x1010FFFF);
        
        // Text inside the frame
        renderTextCentered("Main Menu", 30, width);
    }
}
```

### Example 3: Menu with Contextual Description

```c
void menuContextRender(int screen, int width, int height) {
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        // Option list
        for (int i = 0; i < numOptions; i++) {
            // ...
        }
    }
    
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("--- Description ---", 20, width);
        
        // Display description based on selection
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

### Example 4: Adaptive Screen Rendering (Top/Bottom)

```c
void menuAdaptiveRender(int screen, int width, int height) {
    if (screen == 0) {
        // Top screen: more space (400px)
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTitle(76, 12);  // Centered for 400px
        
        for (int i = 0; i < 6; i++) {
            int x = 100 - strlen(options[i]) * 4;  // For 400px
            renderTextColor(options[i], x, 50 + i*15, colors[i]);
        }
    }
    
    if (screen == 10) {
        // Bottom screen: less space (320px)
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        
        for (int i = 0; i < 4; i++) {
            // Adaptive centering
            int centerX = (width - strlen(options[i]) * 8) / 2;
            renderTextColor(options[i], centerX, 80 + i*30, colors[i]);
        }
    }
}
```

---

## Key Takeaways

✓ **DO:**
- Use `width` for all position calculations
- Adapt layout between screens (top 400px vs bottom 320px)
- Use relative calculations, not hardcoded values
- Test on bottom screen (narrower)
- Use black shadows to improve text readability

✗ **DON'T:**
- Hardcode positions (150, 200, etc.)
- Assume 400px width everywhere
- Let text overflow the screen
- Use accented characters directly in text rendering
- Ignore the `width` parameter in calculations

---
