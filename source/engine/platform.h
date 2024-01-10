#pragma once

#include <stdbool.h>

extern void initPlatform();
extern void exitPlatform();

extern bool canSpeedUp();
extern bool isSpeedUp();
extern void setSpeedUp(bool speedUp);
