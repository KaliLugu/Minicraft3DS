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

// Keyboard
char guiText0[] = "1 2 3 4 5 6 7 8 9 0";
char guiText1[] = "Q W E R T Y U I O P";
char guiText2[] = "A S D F G H J K L";
char guiText3[] = "Z X C V B N M";
char guiText4[] = " SPACE  BACKSPACE";

int touchDelay = 0;
bool isTouching = false;
int touchX = 0, touchY = 0, touchW = 0, touchH = 0;

void menuTickKeyboard(char *string, int maxLength) {
    if ((localInputs.k_touchX != 0 || localInputs.k_touchY != 0) && touchDelay == 0) {
        if (!isTouching) {
            int xVal = localInputs.k_touchX, yVal = localInputs.k_touchY;
            int strLength = strlen(string);
            if (yVal >= 60 && yVal < 80) { // 0 to 9
                if (xVal >= 4 && xVal < 4 + 16) {
                    touchX = 4;
                    if (strLength < maxLength)
                        strcat(string, "1");
                } else if (xVal >= 36 && xVal < 4 + 32 + 16) {
                    touchX = 36;
                    if (strLength < maxLength)
                        strcat(string, "2");
                } else if (xVal >= 68 && xVal < 4 + 64 + 16) {
                    touchX = 68;
                    if (strLength < maxLength)
                        strcat(string, "3");
                } else if (xVal >= 100 && xVal < 4 + 96 + 16) {
                    touchX = 100;
                    if (strLength < maxLength)
                        strcat(string, "4");
                } else if (xVal >= 132 && xVal < 4 + 128 + 16) {
                    touchX = 132;
                    if (strLength < maxLength)
                        strcat(string, "5");
                } else if (xVal >= 164 && xVal < 4 + 160 + 16) {
                    touchX = 164;
                    if (strLength < maxLength)
                        strcat(string, "6");
                } else if (xVal >= 196 && xVal < 4 + 192 + 16) {
                    touchX = 196;
                    if (strLength < maxLength)
                        strcat(string, "7");
                } else if (xVal >= 228 && xVal < 4 + 224 + 16) {
                    touchX = 228;
                    if (strLength < maxLength)
                        strcat(string, "8");
                } else if (xVal >= 260 && xVal < 4 + 256 + 16) {
                    touchX = 260;
                    if (strLength < maxLength)
                        strcat(string, "9");
                } else if (xVal >= 292 && xVal < 4 + 288 + 16) {
                    touchX = 292;
                    if (strLength < maxLength)
                        strcat(string, "0");
                } else
                    return;
                touchY = 56;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if (yVal >= 80 && yVal < 100) { // Q to P
                if (xVal >= 4 && xVal < 4 + 16) {
                    touchX = 4;
                    if (strLength < maxLength)
                        strcat(string, "Q");
                } else if (xVal >= 36 && xVal < 4 + 32 + 16) {
                    touchX = 36;
                    if (strLength < maxLength)
                        strcat(string, "W");
                } else if (xVal >= 68 && xVal < 4 + 64 + 16) {
                    touchX = 68;
                    if (strLength < maxLength)
                        strcat(string, "E");
                } else if (xVal >= 100 && xVal < 4 + 96 + 16) {
                    touchX = 100;
                    if (strLength < maxLength)
                        strcat(string, "R");
                } else if (xVal >= 132 && xVal < 4 + 128 + 16) {
                    touchX = 132;
                    if (strLength < maxLength)
                        strcat(string, "T");
                } else if (xVal >= 164 && xVal < 4 + 160 + 16) {
                    touchX = 164;
                    if (strLength < maxLength)
                        strcat(string, "Y");
                } else if (xVal >= 196 && xVal < 4 + 192 + 16) {
                    touchX = 196;
                    if (strLength < maxLength)
                        strcat(string, "U");
                } else if (xVal >= 228 && xVal < 4 + 224 + 16) {
                    touchX = 228;
                    if (strLength < maxLength)
                        strcat(string, "I");
                } else if (xVal >= 260 && xVal < 4 + 256 + 16) {
                    touchX = 260;
                    if (strLength < maxLength)
                        strcat(string, "O");
                } else if (xVal >= 292 && xVal < 4 + 288 + 16) {
                    touchX = 292;
                    if (strLength < maxLength)
                        strcat(string, "P");
                } else
                    return;
                touchY = 76;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if (yVal >= 100 && yVal < 120) { // A to L
                if (xVal >= 20 && xVal < 36) {
                    touchX = 20;
                    if (strLength < maxLength)
                        strcat(string, "A");
                } else if (xVal >= 52 && xVal < 68) {
                    touchX = 52;
                    if (strLength < maxLength)
                        strcat(string, "S");
                } else if (xVal >= 84 && xVal < 100) {
                    touchX = 84;
                    if (strLength < maxLength)
                        strcat(string, "D");
                } else if (xVal >= 116 && xVal < 132) {
                    touchX = 116;
                    if (strLength < maxLength)
                        strcat(string, "F");
                } else if (xVal >= 148 && xVal < 164) {
                    touchX = 148;
                    if (strLength < maxLength)
                        strcat(string, "G");
                } else if (xVal >= 180 && xVal < 196) {
                    touchX = 180;
                    if (strLength < maxLength)
                        strcat(string, "H");
                } else if (xVal >= 212 && xVal < 230) {
                    touchX = 212;
                    if (strLength < maxLength)
                        strcat(string, "J");
                } else if (xVal >= 244 && xVal < 262) {
                    touchX = 244;
                    if (strLength < maxLength)
                        strcat(string, "K");
                } else if (xVal >= 276 && xVal < 294) {
                    touchX = 276;
                    if (strLength < maxLength)
                        strcat(string, "L");
                } else
                    return;
                touchY = 96;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if (yVal >= 120 && yVal < 140) { // Z to M
                if (xVal >= 52 && xVal < 68) {
                    touchX = 52;
                    if (strLength < maxLength)
                        strcat(string, "Z");
                } else if (xVal >= 84 && xVal < 100) {
                    touchX = 84;
                    if (strLength < maxLength)
                        strcat(string, "X");
                } else if (xVal >= 116 && xVal < 132) {
                    touchX = 116;
                    if (strLength < maxLength)
                        strcat(string, "C");
                } else if (xVal >= 148 && xVal < 164) {
                    touchX = 148;
                    if (strLength < maxLength)
                        strcat(string, "V");
                } else if (xVal >= 180 && xVal < 196) {
                    touchX = 180;
                    if (strLength < maxLength)
                        strcat(string, "B");
                } else if (xVal >= 212 && xVal < 230) {
                    touchX = 212;
                    if (strLength < maxLength)
                        strcat(string, "N");
                } else if (xVal >= 244 && xVal < 262) {
                    touchX = 244;
                    if (strLength < maxLength)
                        strcat(string, "M");
                } else
                    return;
                touchY = 116;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if (yVal >= 140 && yVal < 160) { // SPACE and BACKSPACE
                if (xVal >= 32 && xVal < 112) {
                    touchX = 32;
                    touchW = 16 * 5 + 14;
                    if (strLength < maxLength)
                        strcat(string, "_"); // Underscores don't appear in the game.
                } else if (xVal >= 148 && xVal < 262) {
                    if (strLength <= 0)
                        return;
                    touchX = 148;
                    touchW = 16 * 9 + 8;
                    string[strLength - 1] = '\0';
                } else {
                    return;
                }
                touchY = 136;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            }
        }
    } else if (localInputs.k_touchX == 0 || localInputs.k_touchY == 0) {
        isTouching = false;
    }
    if (touchDelay > 0)
        --touchDelay;
}

void menuRenderKeyboard(int screen, int width, int height) {
    renderTextColor("Touch the keypad", (width / 2 - 16 * 8) / 2, 6, 0xFFFF33FF);

    drawRect(0, 50, 320, 110, 0xBF7F7FFF);

    if (touchDelay > 0) {
        drawRect(touchX, touchY, touchW, touchH, 0xAF0000FF);
    }

    renderText(guiText0, 4, 30);
    renderText(guiText1, 4, 40);
    renderText(guiText2, 12, 50);
    renderText(guiText3, 28, 60);
    renderText(guiText4, 12, 70);
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
