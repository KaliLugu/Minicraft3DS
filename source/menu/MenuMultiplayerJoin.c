#include "MenuMultiplayerJoin.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "../network/Network.h"

int menuScanTimer = 0;

void menuMultiplayerJoinTick() {
    if (menuScanTimer > 0) {
        menuScanTimer--;
    } else {
        networkScan();
        menuScanTimer = 30;
    }
    if (currentSelection >= networkGetScanCount())
        currentSelection = networkGetScanCount() - 1;
    if (currentSelection < 0)
        currentSelection = 0;

    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_CHOOSE_GAME;
        currentSelection = 2;
    }
    if (localInputs.k_up.clicked) {
        --currentSelection;
        if (currentSelection < 0)
            currentSelection = (networkGetScanCount() > 0 ? networkGetScanCount() - 1 : 0);
    }
    if (localInputs.k_down.clicked) {
        ++currentSelection;
        if (currentSelection >= networkGetScanCount())
            currentSelection = 0;
    }

    if (localInputs.k_accept.clicked) {
        if (networkGetScanCount() != 0) {
            for (int t = 0; t < 10; t++) { // try to connect multiple times, because it will not work the first try every time
                if (networkConnect(currentSelection)) {
                    currentMenu = MENU_MULTIPLAYER_WAIT;
                    currentSelection = 0;
                    break;
                }
            }
        }
    }
}

void menuMultiplayerJoinRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        offsetX = 0;
        offsetY = (currentSelection * 32) - 48;
        renderTextCentered("Select a world", -16, width);
        for (int i = 0; i < networkGetScanCount(); ++i) {
            int color = 0x201092FF;
            char *text = malloc((50 + 8 + 1) * sizeof(char));
            memset(text, 0, (50 + 8 + 1) * sizeof(char));
            networkGetScanName(text, i);
            strcat(text, "'s World");

            if (i != currentSelection)
                color &= 0x7F7F7FFF; // Darken color.

            renderFrame(1, i * 4, 24, (i * 4) + 4, color);
            renderTextCentered(text, i * 32 + 12, width);

            free(text);
        }
        offsetX = 0;
        offsetY = 0;
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextColor("Searching for Worlds", (width / 2 - 20 * 8) / 2, 6, 0xFFFF3FFF);
        renderTextCentered("Press  or  to scroll", 25, width);
        renderButtonIcon(localInputs.k_up.input & -localInputs.k_up.input, 40, 20);
        renderButtonIcon(localInputs.k_down.input & -localInputs.k_down.input, 71, 20);
        renderTextCentered("Press   to join", 50, width);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 63, 45);
        renderTextCentered("Press   to return", 75, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 70);
    }
}
