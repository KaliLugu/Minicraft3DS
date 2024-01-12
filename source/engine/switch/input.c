#ifdef __SWITCH__
#include "../engine.h"

#include <string.h>
#include <switch.h>

static PadState pad;
static u64 down;
static u64 held;

static bool check(sInt mask, u64 state) {
    if ((mask & I_A) != 0 && (state & HidNpadButton_A) != 0)
        return true;
    if ((mask & I_B) != 0 && (state & HidNpadButton_B) != 0)
        return true;
    if ((mask & I_X) != 0 && (state & HidNpadButton_X) != 0)
        return true;
    if ((mask & I_Y) != 0 && (state & HidNpadButton_Y) != 0)
        return true;

    if ((mask & I_START_PLUS) != 0 && (state & HidNpadButton_Plus) != 0)
        return true;
    if ((mask & I_SELECT_MINUS) != 0 && (state & HidNpadButton_Minus) != 0)
        return true;

    if ((mask & I_L) != 0 && (state & HidNpadButton_L) != 0)
        return true;
    if ((mask & I_R) != 0 && (state & HidNpadButton_R) != 0)
        return true;
    if ((mask & I_ZL) != 0 && (state & HidNpadButton_ZL) != 0)
        return true;
    if ((mask & I_ZR) != 0 && (state & HidNpadButton_ZR) != 0)
        return true;

    if ((mask & I_DP_UP) != 0 && (state & HidNpadButton_Up) != 0)
        return true;
    if ((mask & I_DP_DOWN) != 0 && (state & HidNpadButton_Down) != 0)
        return true;
    if ((mask & I_DP_LEFT) != 0 && (state & HidNpadButton_Left) != 0)
        return true;
    if ((mask & I_DP_RIGHT) != 0 && (state & HidNpadButton_Right) != 0)
        return true;

    if ((mask & I_SL_UP) != 0 && (state & HidNpadButton_StickLUp) != 0)
        return true;
    if ((mask & I_SL_DOWN) != 0 && (state & HidNpadButton_StickLDown) != 0)
        return true;
    if ((mask & I_SL_LEFT) != 0 && (state & HidNpadButton_StickLLeft) != 0)
        return true;
    if ((mask & I_SL_RIGHT) != 0 && (state & HidNpadButton_StickLRight) != 0)
        return true;

    if ((mask & I_SR_UP) != 0 && (state & HidNpadButton_StickRUp) != 0)
        return true;
    if ((mask & I_SR_DOWN) != 0 && (state & HidNpadButton_StickRDown) != 0)
        return true;
    if ((mask & I_SR_LEFT) != 0 && (state & HidNpadButton_StickRLeft) != 0)
        return true;
    if ((mask & I_SR_RIGHT) != 0 && (state & HidNpadButton_StickRRight) != 0)
        return true;

    return false;
}

void initInput() {
    padInitializeDefault(&pad);
}

void scanInputs() {
    padUpdate(&pad);
    down = padGetButtonsDown(&pad);
    held = padGetButtons(&pad);
}

bool inputIsPressed(sInt mask) {
    return check(mask, held);
}

bool inputWasClicked(sInt mask) {
    return check(mask, down);
}

bool inputHasTouch() {
    return false;
}

sInt inputGetTouchX() {
    return 0;
}

sInt inputGetTouchY() {
    return 0;
}

static TextCallback checkCB;
static char textBuffer[256] = {0};
static SwkbdTextCheckResult validateText(char *text, size_t textBufferSize) {
    if (!checkCB(text)) {
        strncpy(text, "Invalid Input", textBufferSize);
        return SwkbdTextCheckResult_Bad;
    }
    return SwkbdTextCheckResult_OK;
}

char *inputText(char *initial, uShort minLength, uShort maxLength, TextCallback check) {
    checkCB = check;

    SwkbdConfig kb;
    Result result = swkbdCreate(&kb, 0);
    if (R_SUCCEEDED(result)) {
        swkbdConfigMakePresetDefault(&kb);
        swkbdConfigSetTextCheckCallback(&kb, validateText);
        if (initial != NULL)
            swkbdConfigSetInitialText(&kb, initial);
        swkbdConfigSetStringLenMax(&kb, minLength);
        swkbdConfigSetStringLenMax(&kb, maxLength);

        result = swkbdShow(&kb, textBuffer, sizeof(textBuffer));
        swkbdClose(&kb);

        if (R_SUCCEEDED(result)) {
            return textBuffer;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

#endif
