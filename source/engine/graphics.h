#pragma once
#include "dtypes.h"

#define MIN_TEXTURE_SIZE 64

typedef unsigned int MColor;
typedef struct _img _img;
typedef _img *MImage;
typedef struct _tex _tex;
typedef _tex *MTexture;

typedef enum {
    DM_NORMAL = 0,
    DM_MODIFY_SCISSOR,
    DM_SCISSOR,
    DM_SCISSOR_INVERTED
} DrawMode;
typedef void (*DrawCallback)(int screen, int width, int height);

extern void initGraphics();
extern void drawGraphics(DrawCallback callback);
extern void exitGraphics();

extern sInt getScreenCount();

extern MImage createImage(int width, int height);
extern MImage loadImage(char *name);
extern int imageWidth(MImage image);
extern int imageHeight(MImage image);
extern void freeImage(MImage image);

extern MColor getPixel(MImage image, int x, int y);
extern void setPixel(MImage image, int x, int y, MColor color);

extern MTexture createTexture(MImage image, int x0, int y0, int x1, int y1);
inline MTexture createFullTexture(MImage image) {
    return createTexture(image, 0, 0, imageWidth(image), imageHeight(image));
}
extern int textureWidth(MTexture texture);
extern int textureHeight(MTexture texture);
extern void freeTexture(MTexture texture);

extern void drawTextureAt(MTexture texture, float x, float y, float scaleX, float scaleY, float angle, MColor color, float blend);
inline void drawTexture(MTexture texture, float x, float y) {
    drawTextureAt(texture, x, y, 1, 1, 0, 0xFFFFFFFF, 0);
}
extern void drawRect(float x, float y, float w, float h, MColor color);

extern void setDrawMode(DrawMode mode);
extern void setClearColor(MColor color);
