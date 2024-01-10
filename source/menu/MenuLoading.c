#include "MenuLoading.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"

void menuLoadingTick() {
}

void menuLoadingRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextCentered("Loading game...", 50, width);
    }
}
