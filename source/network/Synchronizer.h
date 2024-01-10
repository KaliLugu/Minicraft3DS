#pragma once

#include <stdbool.h>
#include <stddef.h>

// 2-3 seem be optimal (at least for 2 players)
#define SYNCHRONIZER_TICKS_PER_TURN 2

extern void synchronizerInit(int seed, int initPlayerCount, int initPlayerLocalID);

extern void synchronizerSendUID();
extern void synchronizerSetPlayerUID(int playerID, int uid);
extern void synchronizerSendIfReady();
extern void synchronizerSetPlayerReady(int playerID);
extern bool synchronizerAllReady();
extern bool synchronizerIsRunning();

extern void synchronizerStart();

extern void synchronizerTick(void (*gtick)(void));

extern void synchronizerReset();

extern void synchronizerOnInputPacket(int playerID, int turnNumber, void *data, size_t dataSize);

// values used ingame
extern int syncTickCount;

// helpers for random numbers
extern double gaussrand(bool reset);
