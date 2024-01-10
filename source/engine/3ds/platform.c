#ifdef __3DS__
#include "../engine.h"
#include <3ds.h>

static u8 MODEL_3DS;
static bool shouldSpeedup = false;

void initPlatform() {
    cfguInit();
    CFGU_GetSystemModel(&MODEL_3DS);
    romfsInit();
}
void exitPlatform() {
    romfsExit();
    cfguExit();
}

bool canSpeedUp() {
    return ((MODEL_3DS & 6) != 0);
}
bool isSpeedUp() {
    return shouldSpeedup;
}
void setSpeedUp(bool speedUp) {
    shouldSpeedup = speedUp;
    osSetSpeedupEnable(shouldSpeedup);
}

void runMainLoop(TickCallback tickCB, DrawCallback drawCB) {
    while (aptMainLoop()) {
        if (!tickCB())
            break;

        drawGraphics(drawCB);
    }
}

#endif