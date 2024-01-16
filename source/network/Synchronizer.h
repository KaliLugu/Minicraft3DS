#pragma once

#include "../engine/dtypes.h"
#include <stddef.h>

// 2-3 seem be optimal (at least for 2 players)
#define SYNCHRONIZER_TICKS_PER_TURN 2

extern void synchronizerStartSP();
extern void synchronizerStartMPHost();
extern void synchronizerStartMPClient();

extern bool synchronizerIsRunning();
extern void synchronizerTick(void (*gtick)(void));
extern void synchronizerReset();

// values used ingame
extern int syncTickCount;

// helpers for random numbers
#define SYNC_RAND_MAX 32767
extern sInt syncRand();
extern double syncGaussRand();
