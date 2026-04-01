#pragma once

#include "../SaveLoad.h"

extern void MenuErrorLoadingInit(int errorCode);
extern void MenuErrorLoadingTick();
extern void MenuErrorLoadingRender(int screen, int width, int height);
