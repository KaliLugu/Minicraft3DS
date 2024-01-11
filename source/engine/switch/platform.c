#ifdef __SWITCH__
#include "../engine.h"
#include <switch.h>

void initPlatform() {
    romfsInit();
}
void exitPlatform() {
    romfsExit();
}

bool canSpeedUp() {
    return false;
}

bool isSpeedUp() {
    return false;
}

void setSpeedUp(bool speedUp) {
}

void runMainLoop(TickCallback tickCB, DrawCallback drawCB) {
    while (appletMainLoop()) {
        if (!tickCB())
            break;

        drawGraphics(drawCB);
    }
}

#endif
