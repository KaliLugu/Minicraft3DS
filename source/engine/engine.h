#pragma once
#include "audio.h"
#include "graphics.h"

// TODO: why is thie deplicate required from graphics.h?
typedef void (*DrawCallback)(int screen, int width, int height);

extern void initEngine(DrawCallback cb);
extern void exitEngine();
