#ifdef __3DS__
#include "../engine.h"

#include <3ds.h>

static u32 down;
static u32 held;
static touchPosition tp;

static bool check(sInt mask, u32 state) {
    if ((mask & I_A) != 0 && (state & KEY_A) != 0)
        return true;
    if ((mask & I_B) != 0 && (state & KEY_B) != 0)
        return true;
    if ((mask & I_X) != 0 && (state & KEY_X) != 0)
        return true;
    if ((mask & I_Y) != 0 && (state & KEY_Y) != 0)
        return true;

    if ((mask & I_START_PLUS) != 0 && (state & KEY_START) != 0)
        return true;
    if ((mask & I_SELECT_MINUS) != 0 && (state & KEY_SELECT) != 0)
        return true;

    if ((mask & I_L) != 0 && (state & KEY_L) != 0)
        return true;
    if ((mask & I_R) != 0 && (state & KEY_R) != 0)
        return true;
    if ((mask & I_ZL) != 0 && (state & KEY_ZL) != 0)
        return true;
    if ((mask & I_ZR) != 0 && (state & KEY_ZR) != 0)
        return true;

    if ((mask & I_DP_UP) != 0 && (state & KEY_DUP) != 0)
        return true;
    if ((mask & I_DP_DOWN) != 0 && (state & KEY_DDOWN) != 0)
        return true;
    if ((mask & I_DP_LEFT) != 0 && (state & KEY_DLEFT) != 0)
        return true;
    if ((mask & I_DP_RIGHT) != 0 && (state & KEY_DRIGHT) != 0)
        return true;

    if ((mask & I_SL_UP) != 0 && (state & KEY_CPAD_UP) != 0)
        return true;
    if ((mask & I_SL_DOWN) != 0 && (state & KEY_CPAD_DOWN) != 0)
        return true;
    if ((mask & I_SL_LEFT) != 0 && (state & KEY_CPAD_LEFT) != 0)
        return true;
    if ((mask & I_SL_RIGHT) != 0 && (state & KEY_CPAD_RIGHT) != 0)
        return true;

    if ((mask & I_SR_UP) != 0 && (state & KEY_CSTICK_UP) != 0)
        return true;
    if ((mask & I_SR_DOWN) != 0 && (state & KEY_CSTICK_DOWN) != 0)
        return true;
    if ((mask & I_SR_LEFT) != 0 && (state & KEY_CSTICK_LEFT) != 0)
        return true;
    if ((mask & I_SR_RIGHT) != 0 && (state & KEY_CSTICK_RIGHT) != 0)
        return true;

    return false;
}

void initInput() {
}

void scanInputs() {
    hidScanInput();
    down = hidKeysDown();
    held = hidKeysHeld();
    hidTouchRead(&tp);
}

bool inputIsPressed(sInt mask) {
    return check(mask, held);
}

bool inputWasClicked(sInt mask) {
    return check(mask, down);
}

bool inputHasTouch() {
    return true;
}

sInt inputGetTouchX() {
    return tp.px;
}

sInt inputGetTouchY() {
    return tp.py;
}

#endif