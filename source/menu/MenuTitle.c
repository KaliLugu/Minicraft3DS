#include "MenuTitle.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "MenuLoadGame.h"
#include "MenuTutorial.h"

#include "../render/TextureManager.h"

char options[][12] = {"Start Game", "Editor", "How To Play", "Settings", "About", "Exit"};

void menuTitleTick() {
    menuUpdateMapBG();

    if (localInputs.k_up.clicked) {
        --currentSelection;
        if (currentSelection < 0)
            currentSelection = 5;
    }
    if (localInputs.k_down.clicked) {
        ++currentSelection;
        if (currentSelection > 5)
            currentSelection = 0;
    }

    if (localInputs.k_accept.clicked) {
        switch (currentSelection) {
        case 0:
            currentSelection = 0;
            currentMenu = MENU_CHOOSE_GAME;
            break;
        case 1:
            currentSelection = 0;
            currentMenu = MENU_LOADGAME;
            menuLoadGameInit(LOAD_TO_EDITOR);
            break;
        case 2:
            currentMenu = MENU_TUTORIAL;
            menuTutorialInit();
            break;
        case 3:
            currentSelection = 0;
            currentMenu = MENU_SETTINGS;
            break;
        case 4:
            currentMenu = MENU_ABOUT;
            break;
        case 5:
            quitGame = true;
            break;
        }
    }
}

void menuTitleRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        menuRenderMapBGTop();

        renderTitle(76, 12);

        for (int i = 5; i >= 0; --i) {
            char *msg = options[i];
            Color color = 0x7F7F7FFF;
            if (i == currentSelection)
                color = 0xFFFFFFFF;
            renderTextColor(msg, ((200 - (strlen(msg) * 8)) / 2) + 1, (i * 10 + 55) + 1, 0x000000FF);
            renderTextColor(msg, (200 - (strlen(msg) * 8)) / 2, i * 10 + 55, color);
        }

        renderText(versionText, 2, 225);
    }

    /* Bottom Screen */
    if (screen == 10) {
        menuRenderMapBGBottom();

        int startX = 0, startY = 0; // relative coordinates ftw
        switch (currentSelection) {
        case 0: // "Start Game"

            break;
        case 1: // "Editor"

            break;
        case 2: // "How To Play"
            renderTextColor("Learn the basics", (160 - (16 * 8)) / 2, 12, 0xFFFF7FFF);
            startX = 72;
            startY = 54 + 8;
            renderTile16(startX, startY, 6, 13, 0); // C-PAD
            startX = 72;
            startY = 37 + 8;
            renderTile16(startX, startY - 16, 1, 7, 0); // Player
            renderTile16(startX, startY, 7, 13, 0);     // C-PAD up
            startX = 72;
            startY = 71 + 8;
            renderTile16(startX, startY + 16, 0, 7, 0); // Player
            renderTile16(startX, startY, 9, 13, 0);     // C-PAD down
            startX = 39;
            startY = 54 + 8;
            renderTile16(startX, startY, 3, 7, 1);       // Player
            renderTile16(startX + 16, startY, 8, 13, 0); // C-PAD left
            startX = 89;
            startY = 54 + 8;
            renderTile16(startX + 16, startY, 3, 7, 0); // Player
            renderTile16(startX, startY, 10, 13, 0);    // C-PAD right
            break;
        case 3: // "Settings"
            renderTextColor("Modify the game", (160 - (15 * 8)) / 2, 12, 0xFFFF7FFF);
            renderTile16(48 + 0, 48, 0, 7, 0);
            renderTile16(48 + 16, 48, 1, 7, 0);
            renderTile16(48 + 32, 48, 2, 7, 0);
            renderTile16(48 + 48, 48, 3, 7, 0);
            break;
        case 4: // "About"
            renderTextColor("Who made this game?", (160 - (19 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 5: // "Exit"
            renderTextColor("Exit the game", (160 - (13 * 8)) / 2, 12, 0xFFFF7FFF);
            renderTextColor("(bye-bye)", (160 - (9 * 8)) / 2, 100, 0xFFFF7FFF);
            break;
        }
    }
}
