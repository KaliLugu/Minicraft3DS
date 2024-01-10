#pragma once

#include "Player.h"

extern void ingameMenuTick(PlayerData *pd, int menu);
extern void ingameMenuRender(PlayerData *pd, int menu, int screen, int width, int height);

extern void ingameMenuTickTouch(PlayerData *pd);
