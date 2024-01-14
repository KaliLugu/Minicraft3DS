#include "MenuMultiplayerHost.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "MenuLoadGame.h"

void menuMultiplayerHostTick() {
    if (localInputs.k_decline.clicked) {
        networkDisconnect();

        currentMenu = MENU_CHOOSE_GAME;
        currentSelection = 1;
    }

    if (localInputs.k_accept.clicked) {
        if (networkGetNodeCount() > 1) {
            currentMenu = MENU_LOADGAME;
            menuLoadGameInit(LOAD_TO_MULTIPLAYER);
            currentSelection = 0;
        }
    }
}

void menuMultiplayerHostRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("Connected Players", 4, width);
        for (int i = 0; i < networkGetNodeCount(); i++) {
            char *text = malloc((50 + 8 + 1) * sizeof(char));
            memset(text, 0, (50 + 8 + 1) * sizeof(char));
            networkGetNodeName(i, text);

            renderTextCentered(text, i * 13 + 16, width);

            free(text);
        }
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("Press   to start", 50, width);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 59, 45);
        renderTextCentered("Press   to return", 75, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 70);
    }
}
