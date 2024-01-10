#pragma once

#include <3ds.h>

extern void startGame(bool load, char *filename);
extern void tickGame();
extern void renderGame(int screen, int width, int height);

extern void exitGame();
