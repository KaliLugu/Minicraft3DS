#pragma once
#include "dtypes.h"

#define MIN_TEXTURE_SIZE 64

typedef unsigned int Color;
typedef struct _img _img;
typedef _img *Image;
typedef struct _tex _tex;
typedef _tex *Texture;

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

extern Image createImage(int width, int height);
extern Image loadImage(char *name);
extern int imageWidth(Image image);
extern int imageHeight(Image image);
extern void freeImage(Image image);

extern Color getPixel(Image image, int x, int y);
extern void setPixel(Image image, int x, int y, Color color);

extern Texture createTexture(Image image, int x0, int y0, int x1, int y1);
inline Texture createFullTexture(Image image) {
    return createTexture(image, 0, 0, imageWidth(image), imageHeight(image));
}
extern int textureWidth(Texture texture);
extern int textureHeight(Texture texture);
extern void freeTexture(Texture texture);

extern void drawTextureAt(Texture texture, float x, float y, float scaleX, float scaleY, float angle, Color color, float blend);
inline void drawTexture(Texture texture, float x, float y) {
    drawTextureAt(texture, x, y, 1, 1, 0, 0xFFFFFFFF, 0);
}
extern void drawRect(float x, float y, float w, float h, Color color);

extern void setDrawMode(DrawMode mode);
extern void setClearColor(Color color);
