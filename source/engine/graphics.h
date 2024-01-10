#pragma once

#include "3ds/graphics.h"

typedef enum {
    DM_NORMAL = 0,
    DM_MODIFY_SCISSOR,
    DM_SCISSOR,
    DM_SCISSOR_INVERTED
} DrawMode;
typedef void (*DrawCallback)(int screen, int width, int height);

extern void initGraphics(DrawCallback callback);
extern void drawGraphics();
extern void exitGraphics();

extern Image createImage(int width, int height);
extern Image loadImage(char *name);
extern int imageWidth(Image image);
extern int imageHeight(Image image);
extern void freeImage(Image image);

extern Color getPixel(Image image, int x, int y);
extern void setPixel(Image image, int x, int y, Color color);

extern void createTexture(Texture *texture, Image image, int x0, int y0, int x1, int y1);
inline void createFullTexture(Texture *texture, Image image) {
    return createTexture(texture, image, 0, 0, imageWidth(image), imageHeight(image));
}
extern int textureWidth(Texture *texture);
extern int textureHeight(Texture *texture);
extern void freeTexture(Texture *texture);

extern void drawTextureAt(Texture *texture, float x, float y, float scaleX, float scaleY, float angle, Color color, float blend);
inline void drawTexture(Texture *texture, float x, float y) {
    drawTextureAt(texture, x, y, 1, 1, 0, 0xFFFFFFFF, 0);
}
extern void drawRect(float x, float y, float w, float h, Color color);

extern void setDrawMode(DrawMode mode);
extern void setClearColor(Color color);
