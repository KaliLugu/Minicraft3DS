#pragma once

#include "engine/engine.h"

// only down and clicked need to be send, input is for config stuff
typedef struct
{
    bool down, clicked;
    sInt input;
} Key;

typedef struct
{
    Key k_null;
    Key k_up;
    Key k_down;
    Key k_left;
    Key k_right;
    Key k_attack;
    Key k_menu;
    Key k_pause;
    Key k_accept;
    Key k_decline;
    Key k_delete;
    Key k_menuNext;
    Key k_menuPrev;
    sInt k_touchX;
    sInt k_touchY;
} Inputs;

extern Inputs localInputs;

extern void tickKeys(Inputs *inputs);
extern void resetKeys(Inputs *inputs);
extern void resetClicked(Inputs *inputs);
