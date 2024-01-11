#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "audio.h"
#include "dtypes.h"
#include "graphics.h"
#include "input.h"
#include "network.h"
#include "platform.h"

#ifdef __3DS__
#define AUDIOFILES ".opus"
#endif
#ifdef __SWITCH__
#define AUDIOFILES ".ogg"
#endif

typedef bool (*TickCallback)();

extern void initEngine(PacketHandler ph);
extern void runMainLoop(TickCallback tickCB, DrawCallback drawCB);
extern void exitEngine();
