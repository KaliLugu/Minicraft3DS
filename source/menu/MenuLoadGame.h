#pragma once

#define LOAD_TO_SINGLEPLAYER 1
#define LOAD_TO_MULTIPLAYER 2
#define LOAD_TO_EDITOR 3

extern void menuLoadGameInit(int target);
extern void menuLoadGameTick();
extern void menuLoadGameRender();
