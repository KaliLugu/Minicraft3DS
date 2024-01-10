#pragma once
#include "dtypes.h"
#include <stdbool.h>

enum Input {
    I_A = 0x1,
    I_B = 0x2,
    I_X = 0x4,
    I_Y = 0x8,

    I_START_PLUS = 0x10,
    I_SELECT_MINUS = 0x20,

    I_L = 0x100,
    I_R = 0x200,
    I_ZL = 0x400,
    I_ZR = 0x800,

    I_DP_UP = 0x1000,
    I_DP_DOWN = 0x2000,
    I_DP_LEFT = 0x4000,
    I_DP_RIGHT = 0x8000,

    I_SL_UP = 0x10000,
    I_SL_DOWN = 0x20000,
    I_SL_LEFT = 0x40000,
    I_SL_RIGHT = 0x80000,

    I_SR_UP = 0x100000,
    I_SR_DOWN = 0x200000,
    I_SR_LEFT = 0x400000,
    I_SR_RIGHT = 0x800000
};

extern void scanInputs();
extern bool inputIsPressed(sInt mask);
extern bool inputWasClicked(sInt mask);

extern bool inputHasTouch();
extern sInt inputGetTouchX();
extern sInt inputGetTouchY();
