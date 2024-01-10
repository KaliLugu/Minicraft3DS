#include "EditorOptions.h"

#include "../Menu.h"
#include "../Render.h"
#include "../SaveLoad.h"

#include <string.h>

char editorOptions[][24] = {"Save", "Rename", "Return to title"};

int editorOptionsSelection;

bool editorOptionsChangeName;
char editorOrigFileName[256];
char editorCurrentFileName[256];
int editorErrorFileName = 0;

bool editorAreYouSure;
bool editorAreYouSureSave;
int editorMenuTimer;

void editorOptionsInit() {
    editorOptionsSelection = 0;
    editorOptionsChangeName = false;

    strcpy(editorOrigFileName, currentFileName);

    editorAreYouSure = false;
    editorAreYouSureSave = false;
    editorMenuTimer = 0;
}

void editorOptionsTick() {
    // changing filename
    if (editorOptionsChangeName) {
        if (localInputs.k_decline.clicked)
            editorOptionsChangeName = false;
        if (localInputs.k_accept.clicked && editorErrorFileName == 0) {
            editorErrorFileName = checkFileNameForErrors(editorCurrentFileName);
            if (editorErrorFileName == 0) {       // If no errors are found with the filename, then start a new game!
                memset(&currentFileName, 0, 255); // reset currentFileName
                sprintf(currentFileName, "%s.msv", editorCurrentFileName);
                editorOptionsChangeName = false;
            }
        }

        menuTickKeyboard(editorCurrentFileName, 24);
        if (localInputs.k_touchX > 0 || localInputs.k_touchY > 0) {
            editorErrorFileName = 0;
        }
        // normal menu
    } else {
        if (editorAreYouSureSave) {
            if (localInputs.k_accept.clicked) {
                // changed the filename -> copy old save so playerdata will be kept
                if (strcmp(editorOrigFileName, currentFileName) != 0) {
                    if (!saveFileCopy(currentFileName, editorOrigFileName)) {
                        return;
                    }
                }
                saveWorld(currentFileName, &eManager, &worldData, players, playerCount);

                editorMenuTimer = 60;

                editorAreYouSureSave = false;
            } else if (localInputs.k_decline.clicked) {
                editorAreYouSureSave = false;
            }
        } else if (editorAreYouSure) {
            if (localInputs.k_accept.clicked) {
                currentMenu = MENU_TITLE;
                currentSelection = 1;
            } else if (localInputs.k_decline.clicked) {
                editorAreYouSure = false;
            }
        } else {
            if (localInputs.k_up.clicked) {
                --editorOptionsSelection;
                if (editorOptionsSelection < 0)
                    editorOptionsSelection = 2;
            }
            if (localInputs.k_down.clicked) {
                ++editorOptionsSelection;
                if (editorOptionsSelection > 2)
                    editorOptionsSelection = 0;
            }

            if (localInputs.k_accept.clicked) {
                switch (editorOptionsSelection) {
                case 0:
                    editorAreYouSureSave = true;
                    break;
                case 1:
                    editorOptionsChangeName = true;
                    // copy filename without .msv
                    strncpy(editorCurrentFileName, currentFileName, strlen(currentFileName) - 4);
                    editorErrorFileName = 0;
                    break;
                case 2:
                    editorAreYouSure = true;
                    break;
                }
            }
        }
    }

    if (editorMenuTimer > 0)
        editorMenuTimer--;
}

void editorOptionsRenderTop(int screen, int width, int height) {
    // changing filename
    if (editorOptionsChangeName) {
        renderTextCentered("Enter a name", 52, width);
        renderTextCentered(editorCurrentFileName, 68, width);

        if (editorErrorFileName > 0) {
            switch (editorErrorFileName) { // Error: Filename cannot already exist.
            case 1:
                renderTextColor("Length cannot be 0!", (width / 2 - 19 * 8) / 2, 100, 0xaF1010FF);
                break;
            case 2:
                renderTextColor("You need Letters/Numbers!", (width / 2 - 25 * 8) / 2, 100, 0xAF1010FF);
                break;
            case 3:
                renderTextColor("Filename already exists!", (width / 2 - 24 * 8) / 2, 100, 0xAF1010FF);
                break;
            }
        }
        // normal menu
    } else {
        for (int i = 2; i >= 0; --i) {
            char *msg = editorOptions[i];
            Color color = 0x7F7F7FFF;
            if (i == editorOptionsSelection)
                color = 0xFFFFFFFF;
            renderTextColor(msg, (width / 2 - (strlen(msg) * 8)) / 2, (i * 12) + 56, color);
        }

        if (editorMenuTimer > 0)
            renderTextColor("Game Saved!", (width / 2 - (11 * 8)) / 2, 100, 0x20FF20FF);

        if (editorAreYouSure || editorAreYouSureSave) {
            if (editorAreYouSure)
                renderFrame(5, 5, 20, 12, 0x8F1010FF);
            else
                renderFrame(5, 5, 20, 12, 0x108F10FF);

            renderTextCentered("Are you sure?", 48, width);
            renderTextCentered("   Yes", 64, width);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 83, 64 - 4);
            renderTextCentered("   No", 80, width);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 83, 80 - 4);
        }
    }
}

void editorOptionsRenderBottom(int screen, int width, int height) {
    // changing filename
    if (editorOptionsChangeName) { // Draw the "keyboard"
        menuRenderKeyboard(screen, width, height);

        renderTextCentered("Press   to confirm", 90, width);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 52, 85);
        renderTextCentered("Press   to return", 105, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 100);
        // normal menu
    } else {
    }
}
