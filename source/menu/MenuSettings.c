#include "MenuLoading.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

#include "MenuSettingsRebind.h"
#include "MenuSettingsTexture.h"

char setOptions[][24] = {"Rebind Buttons", "Texture packs", "Debug Text:    ", "N3DS Speedup:    ", "Return to title"};

void menuSettingsTick() {
    if (localInputs.k_up.clicked) {
        --currentSelection;
        if (currentSelection == 3 && !((MODEL_3DS & 6) != 0))
            --currentSelection;
        if (currentSelection < 0)
            currentSelection = 4;
    }
    if (localInputs.k_down.clicked) {
        ++currentSelection;
        if (currentSelection == 3 && !((MODEL_3DS & 6) != 0))
            ++currentSelection;
        if (currentSelection > 4)
            currentSelection = 0;
    }
    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
        currentSelection = 3;
    }
    if (localInputs.k_accept.clicked) {
        switch (currentSelection) {
        case 0:
            menuSettingsRebindInit();
            currentSelection = 0;
            currentMenu = MENU_SETTINGS_REBIND;
            break;
        case 1:
            menuSettingsTextureInit();
            currentMenu = MENU_SETTINGS_TP;
            currentSelection = 0;
            break;
        case 2:
            shouldRenderDebug = !shouldRenderDebug; // toggle option
            break;
        case 3:
            if ((MODEL_3DS & 6) != 0) {         // detect if user is using a New 3DS
                shouldSpeedup = !shouldSpeedup; // toggle option
                osSetSpeedupEnable(shouldSpeedup);
            }
            break;
        case 4:
            if (true) {
                FILE *fset = fopen("settings.bin", "wb");
                fwrite(&shouldRenderDebug, sizeof(bool), 1, fset);
                fwrite(&shouldSpeedup, sizeof(bool), 1, fset);
                fclose(fset);
            }
            currentMenu = MENU_TITLE;
            currentSelection = 3;
            break;
        }
    }
}

void menuSettingsRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderText("Settings", (width / 2 - (8 * 8)) / 2, 15);
        for (int i = 4; i >= 0; --i) {
            char *msg = setOptions[i];
            u32 color = 0x7F7F7FFF;
            if (i == currentSelection)
                color = 0xFFFFFFFF;
            if (i == 2) {
                if (shouldRenderDebug)
                    renderTextColor("On", (width / 2 + 9 * 8) / 2, (8 + i) * 16 - 95, 0x00DF00FF);
                else
                    renderTextColor("Off", (width / 2 + 9 * 8) / 2, (8 + i) * 16 - 95, 0xDF0000FF);
            } else if (i == 3) {

                if ((MODEL_3DS & 6) != 0) { // detect if user is using a New 3DS
                    if (shouldSpeedup)
                        renderTextColor("On", (width / 2 + 11 * 8) / 2, (8 + i) * 16 - 95, 0x00DF00FF);
                    else
                        renderTextColor("Off", (width / 2 + 11 * 8) / 2, (8 + i) * 16 - 95, 0xDF0000FF);
                } else {
                    color = 0xFF3F3F3F;
                    renderTextColor("Off", (width / 2 + 11 * 8) / 2, (8 + i) * 16 - 95, 0x3F3F3FFF);
                }
            }
            renderTextColor(msg, (width / 2 - (strlen(msg) * 8)) / 2, (8 + i) * 16 - 95, color);
        }
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        switch (currentSelection) {
        case 0:
            renderTextColor("Change the controls", (width / 2 - (19 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 1:
            renderTextColor("Change the art", (width / 2 - (14 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 2:
            renderTextColor("Show FPS/Pos/Entity", (width / 2 - (19 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 3:
            renderTextColor("Use N3DS 804mhz mode", (width / 2 - (20 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        case 4:
            renderTextColor("Back to the title", (width / 2 - (17 * 8)) / 2, 12, 0xFFFF7FFF);
            break;
        }
        renderText("Press   to select", 12, 50);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 55, 45);
        renderText("Press   to return", 12, 75);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 70);
    }
}
