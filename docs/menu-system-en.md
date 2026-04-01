# Minicraft3DS Menu System Documentation

## Overview

The Minicraft3DS menu system is a modular architecture that manages all UI screens and interactions in the game. It follows a state-based approach where each menu is a distinct state handled independently with its own tick and render functions.

## Architecture

### Core Menu Flow

```
Main Loop (main.c)
    ├─ tickMenu(currentMenu)     // Process input & logic
    └─ renderMenu(currentMenu)   // Draw to screen
```

The menu system is controlled by a global variable `currentMenu` that holds the ID of the active menu state. Each frame:
1. **Tick Phase**: Input processing and game logic updates
2. **Render Phase**: Visual output to screens

### Menu Definition Structure

Each menu menu follows a consistent pattern with two files:

- **MenuName.h**: Header file with function declarations
- **MenuName.c**: Implementation with two main functions

Example structure:
```c
// MenuName.h
#pragma once
extern void menuNameInit();        // Initialize menu state
extern void menuNameTick();        // Process input & updates
extern void menuNameRender(int screen, int width, int height);  // Draw
```

### Menu IDs

All menu IDs are defined in `Menu.h` with `#define MENU_*` macros:

```c
#define MENU_NONE 0
#define MENU_TITLE 1
#define MENU_CHOOSE_GAME 2
#define MENU_LOADGAME 3
#define MENU_ERROR_LOADING 14
```

## Menu Components

### Tick Function

Called once per frame with user input available. Responsibilities:
- Read input from `localInputs` (keyboard/gamepad processed by input system)
- Update menu state
- Change `currentMenu` to switch to different menu
- Call game initialization functions when entering gameplay

**Inputs structure** (`Inputs.h`):
- `localInputs.k_accept.clicked` - Confirm button
- `localInputs.k_decline.clicked` - Cancel/Back button
- `localInputs.k_up.clicked` / `k_down.clicked` / `k_left.clicked` / `k_right.clicked`
- `localInputs.k_delete.clicked` - Delete/Specialized action

### Render Function

Called once per frame to draw visuals. Parameters:
- `screen`: Which 3DS screen to render to
  - `0`: Top screen
  - `10`: Bottom screen (offset value for 3DS dual-screen system)
- `width`: Screen width in pixels
- `height`: Screen height in pixels

Rendering utilities available:
- `drawRect(x, y, width, height, color)` - Draw colored rectangle
- `renderTextCentered(text, y, width)` - Centered text
- `renderText(text, x, y)` - Positioned text
- `renderTextFitWidth(text, x, y, maxWidth)` - Text with max width

### Color Format

Colors are in RGBA format as 32-bit `unsigned int`:
- `0xRRGGBBAA` or `0x0C0C0CFF` (dark background with full alpha)

## Global State Variables

From `Globals.h`:

```c
extern int currentMenu;          // Current active menu ID
extern int currentSelection;     // Selected item in list menus
extern char currentFileName[256]; // Name of save file being operated on
extern int initGame;             // Signal to start single-player game
extern int initMPGame;           // Signal to start multiplayer game
```

## Menu Transitions

To switch menus, set `currentMenu` to a valid `MENU_*` value:

```c
currentMenu = MENU_TITLE;  // Go to title screen
currentSelection = 0;       // Reset selection
```

## File System Integration

### Loading/Saving

The game stores saves in `.msv` (Minicraft Save) files compressed with ZIP format. Related functions in `SaveLoad.h`:

```c
bool loadWorld(filename, eManager, worldData, players, playerCount);
bool saveWorld(filename, eManager, worldData, players, playerCount);
void getWorldPlayerState(filename, score, win);
```

### Error Codes

Load operations return error codes to help with debugging:

```c
#define LOAD_ERROR_NONE 0              // Successful
#define LOAD_ERROR_VERSION_MISMATCH 1  // Save from different version
#define LOAD_ERROR_LEGACY_SAVE 2       // Old save format
#define LOAD_ERROR_ZIP_FAILED 3        // Corrupted ZIP file
#define LOAD_ERROR_MISSING_WORLD 4     // World data missing
```

Retrieved via: `int errorCode = getLastLoadError();`

## Error Handling Menu

### MenuErrorLoading

Displayed when save file loading encounters issues. Provides:
- Error-specific messages explaining the problem
- Two options: "Continue" (attempt with partial data) or "Back to Menu"

**Functions**:
```c
void MenuErrorLoadingInit(int errorCode);     // Set error code
void MenuErrorLoadingTick();                  // Handle input
void MenuErrorLoadingRender(int screen, int width, int height);  // Display
```

**Error Messages**:
- **Version Mismatch**: "Save file version is incompatible with this version of the game."
- **Legacy Save**: "This is an old save file format. Some data may not be loaded correctly."
- **ZIP Failed**: "Save file is corrupted or cannot be read."
- **Missing World**: "World data is missing from the save file."

**User Interaction Flow**:
1. Player attempts to load a save
2. `loadWorld()` fails and returns to `startGame()` in `Ingame.c`
3. Error code is retrieved
4. `MenuErrorLoadingInit()` called with error code
5. `currentMenu` set to `MENU_ERROR_LOADING`
6. User sees error screen with options
7. **Continue**: Game proceeds with loaded data (if any)
8. **Back**: Returns to `MENU_CHOOSE_GAME`

## Creating a New Menu

### Step 1: Define Menu ID

Add to `Menu.h`:
```c
#define MENU_CUSTOM 20
```

### Step 2: Create Header File

Create `source/menu/MenuCustom.h`:
```c
#pragma once

extern void menuCustomInit();
extern void menuCustomTick();
extern void menuCustomRender(int screen, int width, int height);
```

### Step 3: Create Implementation

Create `source/menu/MenuCustom.c`:
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
        // Handle accept
        currentMenu = MENU_TITLE;
    }
    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
    }
}

void menuCustomRender(int screen, int width, int height) {
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Custom Menu", 20, width);
    }
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Bottom Screen Content", 50, width);
    }
}
```

### Step 4: Register Menu

Add include to `Menu.h`:
```c
#include "menu/MenuCustom.h"
```

Add cases to `Menu.c`:
```c
// In tickMenu()
case MENU_CUSTOM:
    menuCustomTick();
    break;

// In renderMenu()
case MENU_CUSTOM:
    menuCustomRender(screen, width, height);
    break;
```

## Screen Coordinates

3DS has two screens:
- **Top Screen**: 400x240 pixels (screen ID: 0)
- **Bottom Screen**: 320x240 pixels (screen ID: 10)

In render functions, check `screen` parameter to determine which to draw:
```c
if (screen == 0) {
    // Top screen drawing
}
if (screen == 10) {
    // Bottom screen drawing
}
```

## Common Patterns

### List Selection Menu

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
        // Handle selection
    }
}

void render(int screen, int width, int height) {
    if (screen == 10) {
        for (int i = 0; i < itemCount; i++) {
            int y = 30 + (i * 30);
            int color = (i == selectedIndex) ? 0xFFFFFFFF : 0x808080FF;
            // Draw item...
        }
    }
}
```

### Top/Bottom Status Display

```c
void menuCustomRender(int screen, int width, int height) {
    if (screen == 0) {
        // Show status, title, or background on top
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Title", 10, width);
    }
    if (screen == 10) {
        // Show main UI, options, or content on bottom
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Options", 50, width);
    }
}
```

## Save/Load Integration

When implementing a load menu:

```c
void menuCustomTick() {
    if (loadGameRequested) {
        // Attempt to load
        EntityManager eManager = {0};
        WorldData worldData = {0};
        
        if (!loadWorld(filename, &eManager, &worldData, players, playerCount)) {
            // Load failed - show error menu
            int errorCode = getLastLoadError();
            MenuErrorLoadingInit(errorCode);
            currentMenu = MENU_ERROR_LOADING;
            return;
        }
        
        // Load succeeded - start game
        currentMenu = MENU_NONE;
        // Game logic handles rendering in-game
    }
}
```

## Debugging

To add debug output, use functions from `Render.h`:
```c
debugMessage("Menu state: %d", currentMenu);
debug("Custom information");
```

## Performance Notes

- Menus run at 60 FPS cap on 3DS
- Render calls happen for both screens each frame
- Keep tick function logic minimal (heavy calculations before menu switch)
- Background rendering uses `menuRenderMapBG*()` functions for consistent visual style

## Inputs Details

Complete input names available in `Inputs.h`:
- `localInputs.circlepad_x` / `circlepad_y` - Analog stick position
- `localInputs.cstick_x` / `cstick_y` - C-stick position (New 3DS)
- All directional buttons (up, down, left, right)
- Action buttons (accept, decline)
- Shoulder buttons (l, r)
- Specialized buttons (delete, etc.)

Check `.pressed` vs `.clicked` for held vs. pressed-this-frame distinction.
