#ifdef __SWITCH__
#include "../engine.h"

struct _img {
};

struct _tex {
};

void initGraphics() {
}

void drawGraphics(DrawCallback callback) {
}

void exitGraphics() {
}

Image createImage(int width, int height) {
    return NULL;
}

Image loadImage(char *name) {
    return NULL;
}

int imageWidth(Image image) {
    return 0;
}

int imageHeight(Image image) {
    return 0;
}

void freeImage(Image image)[]

    Color getPixel(Image image, int x, int y) {
    return 0;
}

void setPixel(Image image, int x, int y, Color color) {
}

Texture createTexture(Image image, int x0, int y0, int x1, int y1) {
    return NULL;
}

int textureWidth(Texture texture) {
    return 0;
}

int textureHeight(Texture texture) {
    return 0;
}

void freeTexture(Texture texture) {
}

void drawTextureAt(Texture texture, float x, float y, float scaleX, float scaleY, float angle, Color color, float blend) {
}

void drawRect(float x, float y, float w, float h, Color color) {
}

void setDrawMode(DrawMode mode) {
}

void setClearColor(Color color) {
}

#endif
