#include "MenuChooseGame.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "MenuLoadGame.h"

char gameOptions[][24] = {"Singleplayer", "Host Game", "Join Game", "Back"};

void menuChooseGameTick() {
    menuUpdateMapBG();

    if (localInputs.k_up.clicked) {
        --currentSelection;
        if (currentSelection < 0)
            currentSelection = 3;
    }
    if (localInputs.k_down.clicked) {
        ++currentSelection;
        if (currentSelection > 3)
            currentSelection = 0;
    }

    if (localInputs.k_accept.clicked) {
        switch (currentSelection) {
        case 0:
            currentMenu = MENU_LOADGAME;
            menuLoadGameInit(LOAD_TO_SINGLEPLAYER);
            currentSelection = 0;
            break;
        case 1:
            if (networkHost()) {
                currentMenu = MENU_MULTIPLAYER_HOST;
                currentSelection = 0;
            }
            break;
        case 2:
            if (networkScan()) {
                currentMenu = MENU_MULTIPLAYER_JOIN;
                currentSelection = 0;
            }
            break;
        case 3:
            currentMenu = MENU_TITLE;
            currentSelection = 0;
            break;
        }
    }
    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
        currentSelection = 0;
    }
}

void menuChooseGameRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        menuRenderMapBGTop();

        renderTitle(76, 12);

        for (int i = 3; i >= 0; --i) {
            char *msg = gameOptions[i];
            Color color = 0x7F7F7FFF;
            if (i == currentSelection)
                color = 0xFFFFFFFF;
            renderTextColor(msg, ((width / 2 - (strlen(msg) * 8)) / 2) + 1, ((8 + i) * 10 - 16) + 1, 0x000000FF);
            renderTextColor(msg, (width / 2 - (strlen(msg) * 8)) / 2, (8 + i) * 10 - 16, color);
        }

        renderText(versionText, 1, 112);
    }

    /* Bottom Screen */
    if (screen == 10) {
        menuRenderMapBGBottom();

        switch (currentSelection) {
        case 0: // "Start Game"

            break;
        case 1: // "Host Game"
            renderTextColor("local multiplayer", (width / 2 - (17 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 2: // "Join Game"
            renderTextColor("local multiplayer", (width / 2 - (17 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 3: // "Back"
            renderTextColor("Return to the title", (width / 2 - (19 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        }
    }
}
