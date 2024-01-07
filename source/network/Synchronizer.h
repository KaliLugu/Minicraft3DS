#pragma once

#include <3ds.h>

// 2-3 seem be optimal (at least for 2 players)
#define SYNCHRONIZER_TICKS_PER_TURN 2

extern void synchronizerInit(int seed, int initPlayerCount, int initPlayerLocalID);

extern void synchronizerSendUID();
extern void synchronizerSetPlayerUID(int playerID, u32 uid);
extern void synchronizerSendIfReady();
extern void synchronizerSetPlayerReady(int playerID);
extern bool synchronizerAllReady();
extern bool synchronizerIsRunning();

extern void synchronizerStart();

extern void synchronizerTick(void (*gtick)(void));

extern void synchronizerReset();

extern void synchronizerOnInputPacket(u8 playerID, u32 turnNumber, void *data, size_t dataSize);

// values used ingame
extern u32 syncTickCount;

// helpers for random numbers
extern double gaussrand(bool reset);
