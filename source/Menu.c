#include "Menu.h"

#include "Render.h"
#include "editor/Editor.h"

void initMenus() {
    menuInitMapBG();
}

void tickMenu(int menu) {
    switch (menu) {
    case MENU_TITLE:
        menuTitleTick();
        break;
    case MENU_CHOOSE_GAME:
        menuChooseGameTick();
        break;
    case MENU_LOADGAME:
        menuLoadGameTick();
        break;
    case MENU_MULTIPLAYER_HOST:
        menuMultiplayerHostTick();
        break;
    case MENU_MULTIPLAYER_JOIN:
        menuMultiplayerJoinTick();
        break;
    case MENU_MULTIPLAYER_WAIT:
        menuMultiplayerWaitTick();
        break;
    case MENU_LOADING:
        menuLoadingTick();
        break;
    case MENU_EDITOR:
        editorTick();
        break;
    case MENU_TUTORIAL:
        menuTutorialTick();
        break;
    case MENU_SETTINGS:
        menuSettingsTick();
        break;
    case MENU_SETTINGS_REBIND:
        menuSettingsRebindTick();
        break;
    case MENU_SETTINGS_TP:
        menuSettingsTextureTick();
        break;
    case MENU_ABOUT:
        menuAboutTick();
        break;
    }
}

void renderMenu(int menu, int screen, int width, int height) {
    switch (menu) {
    case MENU_TITLE:
        menuTitleRender(screen, width, height);
        break;
    case MENU_CHOOSE_GAME:
        menuChooseGameRender(screen, width, height);
        break;
    case MENU_LOADGAME:
        menuLoadGameRender(screen, width, height);
        break;
    case MENU_MULTIPLAYER_HOST:
        menuMultiplayerHostRender(screen, width, height);
        break;
    case MENU_MULTIPLAYER_JOIN:
        menuMultiplayerJoinRender(screen, width, height);
        break;
    case MENU_MULTIPLAYER_WAIT:
        menuMultiplayerWaitRender(screen, width, height);
        break;
    case MENU_LOADING:
        menuLoadingRender(screen, width, height);
        break;
    case MENU_EDITOR:
        editorRender(screen, width, height);
        break;
    case MENU_TUTORIAL:
        menuTutorialRender(screen, width, height);
        break;
    case MENU_SETTINGS:
        menuSettingsRender(screen, width, height);
        break;
    case MENU_SETTINGS_REBIND:
        menuSettingsRebindRender(screen, width, height);
        break;
    case MENU_SETTINGS_TP:
        menuSettingsTextureRender(screen, width, height);
        break;
    case MENU_ABOUT:
        menuAboutRender(screen, width, height);
        break;
    }
}

// Menu Map Background
bool menuHasMapLoaded = false;
float mxscr = 400;
float myscr = 400;
float menuxa = 0;
float menuya = 0;

void menuInitMapBG() {
    menuHasMapLoaded = true;
    while (menuxa == 0)
        menuxa = (rand() % 3 - 1) * 0.25;
    while (menuya == 0)
        menuya = (rand() % 3 - 1) * 0.25;
}

void menuUpdateMapBG() {
    if (menuHasMapLoaded) {
        mxscr += menuxa;
        myscr += menuya;

        if (mxscr < 16) {
            mxscr = 16;
            menuxa = -menuxa;
        } else if (mxscr > 1832) {
            mxscr = 1832;
            menuxa = -menuxa;
        }
        if (myscr < 16) {
            myscr = 16;
            menuya = -menuya;
        } else if (myscr > 1792) {
            myscr = 1792;
            menuya = -menuya;
        }
    }
}

void menuRenderMapBGTop() {
    if (menuHasMapLoaded) {
        offsetX = (int)mxscr;
        offsetY = (int)myscr;
        renderBackground(1, (int)mxscr, (int)myscr);
        offsetX = 0;
        offsetY = 0;

        drawRect(0, 0, 400, 240, 0x0C0C0CAA);
    }
}

void menuRenderMapBGBottom() {
    if (menuHasMapLoaded) {
        offsetX = (int)mxscr + 20;
        offsetY = (int)myscr + 120;
        renderBackground(1, (int)mxscr + 20, (int)myscr + 120);
        offsetX = 0;
        offsetY = 0;

        drawRect(0, 0, 320, 240, 0x0C0C0CAA);
    }
}
