#include "MenuMultiplayerHost.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "MenuLoadGame.h"

void menuChangelogTick() {
    if (localInputs.k_accept.clicked) {
        currentSelection = 0;
        currentMenu = MENU_TITLE;
    }
}

void menuChangelogRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
        renderTextCentered("Changelog 2.1.0", 0, width);

        renderText("Gameplay", 0, 20);
        renderText("-Added effect scrolls", 0, 30);
        renderText("-Added beds", 0, 40);

        renderText("Quality of Life", 0, 50);
        renderText("-Fixed save freezing", 0, 60);
        renderText("-Version Gard", 0, 70);
        renderText("-Bug fixes", 0, 80);


        renderText("Technical", 0, 90);
        renderText("-Refactored item system", 0, 100);
        renderText("-Added version manager", 0, 110);
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("Press   to continue", 50, width);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 48, 45);
    }
}
