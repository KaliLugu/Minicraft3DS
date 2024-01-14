#include "MenuMultiplayerWait.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

void menuMultiplayerWaitTick() {
    if (localInputs.k_decline.clicked) {
        networkDisconnect();

        currentMenu = MENU_TITLE;
        currentSelection = 2;
    }
}

void menuMultiplayerWaitRender(int screen, int width, int height) {
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
        renderTextCentered("Waiting for host to start", 3 * 13 + 16, width);
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("Awaiting game start..", 50, width);
        renderTextCentered("Press   to return", 75, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 70);
    }
}
