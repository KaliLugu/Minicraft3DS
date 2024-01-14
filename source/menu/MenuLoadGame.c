#include "MenuLoadGame.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include <dirent.h>
#include <stdio.h>

#include "../SaveLoad.h"
#include "../editor/Editor.h"
#include "../network/Synchronizer.h"

static char fileNames[1000][256];
static sInt fileScore[1000];
static bool fileWin[1000];
static int worldFileCount = 0;
static int errorFileName = 0;

static int loadGameTarget;

static bool enteringName = false;
static bool areYouSure = false;

static void readFiles() {
    memset(&fileNames, 0, sizeof(fileNames)); // reset fileNames
    worldFileCount = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strncmp(dir->d_name + strlen(dir->d_name) - 4, ".msv", 4) == 0) { // Check if filename contains ".msv"
                strncpy(fileNames[worldFileCount], dir->d_name, strlen(dir->d_name) - 4);

                getWorldPlayerState(dir->d_name, &fileScore[worldFileCount], &fileWin[worldFileCount]);

                ++worldFileCount;
            }
        }
        closedir(d);
    }
}

static void createOrLoad(char *name) {
    if (name == NULL)
        return;

    memset(&currentFileName, 0, 255); // reset currentFileName
    sprintf(currentFileName, "%s.msv", name);
    playSound(snd_test);

    if (loadGameTarget == LOAD_TO_SINGLEPLAYER) {
        initGame = 1;
        currentMenu = MENU_LOADING;
    } else if (loadGameTarget == LOAD_TO_MULTIPLAYER) {
        initMPGame = 2;
    } else if (loadGameTarget == LOAD_TO_EDITOR) {
        editorInit();
        currentMenu = MENU_EDITOR;
    }
}

void menuLoadGameInit(int target) {
    currentSelection = 0;
    areYouSure = false;

    readFiles();
    if (worldFileCount > 0) {
        memset(&currentFileName, 0, 255); // reset currentFileName
        sprintf(currentFileName, "%s.msv", fileNames[currentSelection]);

        initBGMap = 1;
    } else {
        initBGMap = 2;
    }

    loadGameTarget = target;
}

void menuLoadGameTick() {
    if (enteringName) {
        char *name = inputText("", 1, 20, isWorldNameValid);
        enteringName = false;
        if (name != NULL && checkFileNameForErrors(name) == 0) {
            ++worldFileCount;
            createOrLoad(name);
        }
    }

    if (!areYouSure) { // World select
        if (localInputs.k_decline.clicked) {
            if (loadGameTarget == LOAD_TO_SINGLEPLAYER) {
                currentMenu = MENU_CHOOSE_GAME;
                currentSelection = 0;
            } else if (loadGameTarget == LOAD_TO_MULTIPLAYER) {
                currentMenu = MENU_MULTIPLAYER_HOST;
            } else {
                currentMenu = MENU_TITLE;
                currentSelection = 1;
            }
        }
        if (localInputs.k_up.clicked) {
            --currentSelection;
            if (currentSelection < 0)
                currentSelection = worldFileCount;
        }
        if (localInputs.k_down.clicked) {
            ++currentSelection;
            if (currentSelection > worldFileCount)
                currentSelection = 0;
        }

        if (localInputs.k_delete.clicked) {
            if (currentSelection < worldFileCount)
                areYouSure = true;
        }

        if (localInputs.k_accept.clicked) {
            if (currentSelection == worldFileCount) {
                enteringName = true;
            } else {
                createOrLoad(fileNames[currentSelection]);
            }
        }
    } else {
        if (localInputs.k_decline.clicked || localInputs.k_delete.clicked)
            areYouSure = false;
        if (localInputs.k_accept.clicked) {
            sprintf(currentFileName, "%s.msv", fileNames[currentSelection]);
            remove(currentFileName);
            readFiles();
            areYouSure = false;
            memset(&currentFileName, 0, 255); // reset currentFileName
        }
    }
}

void menuLoadGameRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        if (!enteringName) { // World select
            offsetX = 0;
            offsetY = (currentSelection * 32) - 48;
            renderTextCentered("Select a file", -16, width);
            for (int i = 0; i < worldFileCount + 1; ++i) {
                int color = 0x201092FF;
                char *text = fileNames[i];
                if (i == worldFileCount) {
                    text = "Generate New World";
                    color = 0x109220FF;
                }
                if (i != currentSelection)
                    color &= 0x7F7F7FFF; // Darken color.
                else {
                    if (areYouSure)
                        color = 0xDF1010FF;
                }

                char scoreText[24];
                sprintf(scoreText, "Score: %ld", fileScore[i]);

                renderFrame(1, i * 4, 24, (i * 4) + 4, color);
                if (i != worldFileCount) {
                    renderTextCentered(text, i * 32 + 6, width);
                    renderTextCentered(scoreText, i * 32 + 16, width);
                } else {
                    renderTextCentered(text, i * 32 + 12, width);
                }
                if (fileWin[i] && i != worldFileCount)
                    renderTile16(18, i * 32 + 8, 2, 13, 0); // Render crown
            }
            offsetX = 0;
            offsetY = 0;
        } else { // Enter new world name.
            renderTextCentered("Enter a name", 8, width);
            // renderTextCentered(fileNames[worldFileCount], 24, width);

            if (errorFileName > 0) {
                switch (errorFileName) {
                case 1:
                    renderTextColor("Length cannot be 0!", (width / 2 - 19 * 8) / 2, 100, 0xAF1010FF);
                    break;
                case 2:
                    renderTextColor("You need Letters/Numbers!", (width / 2 - 25 * 8) / 2, 100, 0xAF1010FF);
                    break;
                case 3:
                    renderTextColor("Filename already exists!", (width / 2 - 24 * 8) / 2, 100, 0xAF1010FF);
                    break;
                }
            }
        }
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        if (!enteringName) { // World select
            if (!areYouSure) {
                renderTextColor("Load World", (width / 2 - 10 * 8) / 2, 6, 0xFFFF3FFF);
                renderTextCentered("Press  or  to scroll", 25, width);
                renderButtonIcon(localInputs.k_up.input & -localInputs.k_up.input, 40, 20);
                renderButtonIcon(localInputs.k_down.input & -localInputs.k_down.input, 71, 20);
                renderTextCentered("Press   to select", 50, width);
                renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 55, 45);
                renderTextCentered("Press   to return", 75, width);
                renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 70);
                if (currentSelection != worldFileCount) {
                    renderTextCentered("Press   to delete", 100, width);
                    renderButtonIcon(localInputs.k_delete.input & -localInputs.k_delete.input, 55, 95);
                }
            } else {
                renderTextColor("Delete File?", (width / 2 - 12 * 8) / 2, 6, 0xFF3F3FFF);
                renderTextCentered("Press   to confirm", 50, width);
                renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 52, 45);
                renderTextCentered("Press   to return", 75, width);
                renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 70);
            }
        }
    }
}
