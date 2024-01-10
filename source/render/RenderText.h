#pragma once

#include "../engine/graphics.h"

extern void renderText(char *msg, int x, int y);
extern void renderTextCentered(char *text, int y, int width);
extern void renderTextColor(char *msg, int x, int y, Color color);
extern void renderTextColorSpecial(char *msg, int x, int y, Color color, Color color2);
