#include "MenuAbout.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

void menuAboutTick() {
    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
    }
}

void menuAboutRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("About Minicraft", 6, width);
        renderTextCentered("made by Markus Persson", 18, width);
        renderTextCentered("for the 22'nd ludum dare", 28, width);
        renderTextCentered("competition in dec 2011.", 38, width);
        renderTextCentered("dedicated to my father.<3", 48, width);
        renderTextCentered("- Markus \"Notch\" Persson", 58, width);

        renderTextColor("3DS Homebrew Edition", (width / 2 - 20 * 8) / 2, 80, 0x7FFF7FFF);
        renderTextColor("Remade by Andre Schweiger", (width / 2 - 25 * 8) / 2, 92, 0x00FF00FF);
        renderTextColor("over 2015-2024!", (width / 2 - 15 * 8) / 2, 102, 0x00FF00FF);
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextColor("Special Thanks to:", (width / 2 - 18 * 8) / 2, 6, 0xFF7F7FFF);
        renderTextColor("David Benepe", (width / 2 - 12 * 8) / 2, 16, 0xFF2020FF);
        renderTextColor("(Davideesk) for the", (width / 2 - 19 * 8) / 2, 26, 0xFF2020FF);
        renderTextColor("original 3DS port", (width / 2 - 17 * 8) / 2, 36, 0xFF2020FF);

        renderTextColor("Music from", (width / 2 - 10 * 8) / 2, 56, 0x7FFF7FFF);
        renderTextColor("opengameart.org/", (width / 2 - 16 * 8) / 2, 66, 0x20FF20FF);
        renderTextColor("content/generic-8-", (width / 2 - 18 * 8) / 2, 76, 0x20FF20FF);
        renderTextColor("bit-jrpg-soundtrack", (width / 2 - 19 * 8) / 2, 86, 0x20FF20FF);

        renderTextCentered("Press   to return", 110, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 55, 105);
    }
}
