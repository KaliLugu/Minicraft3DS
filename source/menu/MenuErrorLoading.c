#include "MenuErrorLoading.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"
#include "../Inputs.h"

static int currentErrorCode = LOAD_ERROR_NONE;
static int selectedOption = 0; // 0 = Continue, 1 = Back to menu

static bool canContinueLoadError() {
    return currentErrorCode == LOAD_ERROR_VERSION_MISMATCH || currentErrorCode == LOAD_ERROR_LEGACY_SAVE;
}

void MenuErrorLoadingInit(int errorCode) {
    currentErrorCode = errorCode;
    selectedOption = canContinueLoadError() ? 0 : 1;
}

void MenuErrorLoadingTick() {
    if (localInputs.k_up.clicked || localInputs.k_down.clicked) {
        if (canContinueLoadError()) {
            selectedOption = 1 - selectedOption; // Toggle between 0 and 1
        } else {
            selectedOption = 1;
        }
    }

    if (localInputs.k_accept.clicked) {
        if (selectedOption == 0) {
            // Continue - go back to ingame to try loading with the partial data
            currentMenu = MENU_NONE;
            // The game will proceed with whatever data was loaded
        } else {
            // Back to menu
            currentMenu = MENU_CHOOSE_GAME;
            currentSelection = 0;
        }
    }

    if (localInputs.k_decline.clicked) {
        // Same as Back
        currentMenu = MENU_CHOOSE_GAME;
        currentSelection = 0;
    }
}

void MenuErrorLoadingRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        // Title
        centerAndRender("Error loading game", 20, width);

        // Error message based on error code
        int messageY = 60;
        switch (currentErrorCode) {
        case LOAD_ERROR_VERSION_MISMATCH:
            centerRenderWrapped("Save file version is incompatible with this game.", messageY, width, 20);
            break;
        case LOAD_ERROR_LEGACY_SAVE:
            centerRenderWrapped("Old save file format. Some data may not load correctly.", messageY, width, 20);
            break;
        case LOAD_ERROR_ZIP_FAILED:
            centerRenderWrapped("Save file is corrupted or cannot be read.", messageY, width, 20);
            break;
        case LOAD_ERROR_MISSING_WORLD:
            centerRenderWrapped("World data is missing from the save file.", messageY, width, 20);
            break;
        case LOAD_ERROR_FILE_MISSING:
            centerRenderWrapped("Save file not found. Please choose another file.", messageY, width, 20);
            break;
        default:
            centerRenderWrapped("Unknown error occurred.", messageY, width, 20);
            break;
        }
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        int optionY = 40;

        if (canContinueLoadError()) {
            centerAndRender("Continue", optionY, width);
            if (selectedOption == 0) {
                renderText("<", (width / 2) - 32, optionY);
            }
            centerAndRender("Back to Menu", optionY + 30, width);
            if (selectedOption == 1) {
                renderText("<", (width / 2) - 32, optionY + 30);
            }
        } else {
            centerAndRender("Back to Menu", optionY, width);
            if (selectedOption == 1) {
                renderText("<", (width / 2) - 32, optionY);
            }
        }
    }
}
