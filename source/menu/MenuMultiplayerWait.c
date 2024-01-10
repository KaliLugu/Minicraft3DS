#include "MenuMultiplayerWait.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "../network/Network.h"

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

        networkUpdateStatus();
        renderTextCentered("Connected to", 1 * 13 + 16, width);
        if (networkIsNodeConnected(1)) {
            char *text = malloc((50 + 8 + 1) * sizeof(char));
            memset(text, 0, (50 + 8 + 1) * sizeof(char));
            networkGetNodeName(1, text);

            renderTextCentered(text, 2 * 13 + 16, width);

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
