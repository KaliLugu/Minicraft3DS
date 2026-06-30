#pragma once

#include "engine/dtypes.h"

extern void startGame(bool load, char *filename);
extern void tickGame();
extern void renderGame(int screen, int width, int height);

extern void exitGame();

extern void startFadeToBlack(int duration);
extern void startFadeFromBlack(int duration);
extern void playerSleep();

extern bool isSaving;
