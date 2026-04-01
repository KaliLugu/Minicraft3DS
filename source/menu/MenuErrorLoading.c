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
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        // Title
        renderTextCentered("Error loading game", 20, width);

        // Error message based on error code
        int messageY = 60;
        switch (currentErrorCode) {
        case LOAD_ERROR_VERSION_MISMATCH:
            renderTextCentered("Save file version is", messageY, width);
            renderTextCentered("incompatible with this", messageY + 20, width);
            renderTextCentered("version of the game.", messageY + 40, width);
            break;
        case LOAD_ERROR_LEGACY_SAVE:
            renderTextCentered("This is an old save file", messageY, width);
            renderTextCentered("format. Some data may", messageY + 20, width);
            renderTextCentered("not be loaded correctly.", messageY + 40, width);
            break;
        case LOAD_ERROR_ZIP_FAILED:
            renderTextCentered("Save file is corrupted", messageY, width);
            renderTextCentered("or cannot be read.", messageY + 20, width);
            break;
        case LOAD_ERROR_MISSING_WORLD:
            renderTextCentered("World data is missing", messageY, width);
            renderTextCentered("from the save file.", messageY + 20, width);
            break;
        case LOAD_ERROR_FILE_MISSING:
            renderTextCentered("Save file not found.", messageY, width);
            renderTextCentered("Please choose another file.", messageY + 20, width);
            break;
        default:
            renderTextCentered("Unknown error occurred", messageY, width);
            break;
        }

        // Options
        int optionY = 140;

        if (canContinueLoadError()) {
            renderTextCentered("Continue", optionY, width);
            if (selectedOption == 0) {
                renderTextCentered(">", optionY, width - 40);
            }
            renderTextCentered("Back to Menu", optionY + 30, width);
            if (selectedOption == 1) {
                renderTextCentered(">", optionY + 30, width - 40);
            }
        } else {
            renderTextCentered("Back to Menu", optionY, width);
            if (selectedOption == 1) {
                renderTextCentered(">", optionY, width - 40);
            }
        }
    }
}
