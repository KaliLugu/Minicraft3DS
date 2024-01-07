#pragma once

#include <3ds.h>

extern void renderText(char *msg, u32 x, u32 y);
extern void renderTextSized(char *msg, u32 x, u32 y, float size);
extern void renderTextColor(char *msg, u32 x, u32 y, u32 color);
extern void renderTextColorSized(char *msg, int x, int y, float size, u32 color);
extern void renderTextColorSpecial(char *msg, u32 x, u32 y, u32 color, u32 color2);
