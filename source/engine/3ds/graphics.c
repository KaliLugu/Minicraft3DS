#ifdef __3DS__
#include "../engine.h"

#include <3ds.h>
#include <citro2d.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>

struct _img {
    C3D_Tex tex;
};

struct _tex {
    C2D_Image img;
    bool valid;
};

static MColor clearColor = 0xFF000000;

static C3D_RenderTarget *topScreenTarget;
static C3D_RenderTarget *bottomScreenTarget;

void initGraphics() {
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C2D_SetTintMode(C2D_TintMult);

    topScreenTarget = C3D_RenderTargetCreate(GSP_SCREEN_WIDTH, GSP_SCREEN_HEIGHT_TOP, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(topScreenTarget, GFX_TOP, GFX_LEFT,
                              GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |
                                  GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
                                  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

    bottomScreenTarget = C3D_RenderTargetCreate(GSP_SCREEN_WIDTH, GSP_SCREEN_HEIGHT_BOTTOM, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(bottomScreenTarget, GFX_BOTTOM, GFX_LEFT,
                              GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |
                                  GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
                                  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
}

void drawGraphics(DrawCallback callback) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C3D_RenderTargetClear(topScreenTarget, C3D_CLEAR_ALL, clearColor, 0);
    C2D_SceneBegin(topScreenTarget);
    callback(0, 400, 240);
    C2D_Flush();

    C3D_RenderTargetClear(bottomScreenTarget, C3D_CLEAR_ALL, clearColor, 0);
    C2D_SceneBegin(bottomScreenTarget);
    callback(10, 320, 240);
    C2D_Flush();

    C3D_FrameEnd(0);
}

void exitGraphics() {
    C3D_RenderTargetDelete(topScreenTarget);
    C3D_RenderTargetDelete(bottomScreenTarget);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

sInt getScreenCount() {
    return 2;
}

static inline u32 mortonInterleave(u32 x, u32 y) {
    u32 i = (x & 7) | ((y & 7) << 8); // ---- -210
    i = (i ^ (i << 2)) & 0x1313;      // ---2 --10
    i = (i ^ (i << 1)) & 0x1515;      // ---2 -1-0
    i = (i | (i >> 7)) & 0x3F;
    return i;
}

static inline u32 mortonOffset(u32 x, u32 y, u32 bytesPerPixel) {
    u32 i = mortonInterleave(x, y);
    unsigned int offset = (x & ~7) * 8;
    return (i + offset) * bytesPerPixel;
}

static unsigned int nextPowerOf2(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v >= MIN_TEXTURE_SIZE ? v : MIN_TEXTURE_SIZE;
}

MImage createImage(int width, int height) {
    MImage image = linearAlloc(sizeof(_img));
    if (!C3D_TexInit(&image->tex, nextPowerOf2(width), nextPowerOf2(height), GPU_RGBA8)) {
        linearFree(image);
        return NULL;
    }
    C3D_TexSetWrap(&image->tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
    return image;
}

static MImage loadPng(FILE *file) {
    png_image png;
    png.version = PNG_IMAGE_VERSION;
    png.opaque = NULL;
    if (png_image_begin_read_from_stdio(&png, file) != 0) {
        png.format = PNG_FORMAT_RGBA;
        png_bytep buffer = malloc(PNG_IMAGE_SIZE(png));
        if (buffer != NULL && png_image_finish_read(&png, NULL, buffer, 0, NULL) != 0) {
            MImage image = createImage(png.width, png.height);
            if (image != NULL) {
                // store in morton order
                for (int y = 0; y < png.height; y++) {
                    int coarseY = y & ~7;
                    for (int x = 0; x < png.width; x++) {
                        int offset = mortonOffset(x, y, 4) + coarseY * image->tex.width * 4;

                        MColor v = ((MColor *)buffer)[x + y * png.width];
                        *(MColor *)(image->tex.data + offset) = __builtin_bswap32(v); /* RGBA8 -> ABGR8 */
                    }
                }
            }
            free(buffer);
            return image;
        } else {
            if (buffer == NULL)
                png_image_free(&png);
            else
                free(buffer);
        }
    }
    return NULL;
}

MImage loadImage(char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL)
        return NULL;

    MImage image = loadPng(file);
    fclose(file);
    return image;
}

inline int imageWidth(MImage image) {
    return image->tex.width;
}

inline int imageHeight(MImage image) {
    return image->tex.height;
}

void freeImage(MImage image) {
    if (image == NULL)
        return;
    C3D_TexDelete(&image->tex);
    linearFree(image);
}

MColor getPixel(MImage image, int x, int y) {
    if (image == NULL)
        return 0;

    int coarseY = y & ~7;
    int offset = mortonOffset(x, y, 4) + coarseY * image->tex.width * 4;
    MColor color = *(MColor *)(image->tex.data + offset);
    return color; // TODO: why is this not needed here? __builtin_bswap32(color); /* ABGR8 -> RGBA8 */
}

void setPixel(MImage image, int x, int y, MColor color) {
    if (image == NULL)
        return;

    int coarseY = y & ~7;
    int offset = mortonOffset(x, y, 4) + coarseY * image->tex.width * 4;
    *(MColor *)(image->tex.data + offset) = color; // TODO: why is this not needed here? __builtin_bswap32(color); /* RGBA8 -> ABGR8 */
}

MTexture createTexture(MImage image, int x0, int y0, int x1, int y1) {
    uShort width = x1 - x0;
    uShort height = y1 - y0;
    y0 = imageHeight(image) - y0;
    y1 = imageHeight(image) - y1;
    float top = y0 / (float)image->tex.height;
    float bottom = y1 / (float)image->tex.height;
    float left = x0 / (float)image->tex.width;
    float right = x1 / (float)image->tex.width;
    Tex3DS_SubTexture *subTexture = linearAlloc(sizeof(Tex3DS_SubTexture));
    subTexture->width = width;
    subTexture->height = height;
    subTexture->top = top;
    subTexture->bottom = bottom;
    subTexture->left = left;
    subTexture->right = right;
    C2D_Image c2dImage = {&image->tex, subTexture};
    MTexture texture = linearAlloc(sizeof(_tex));
    texture->img = c2dImage;
    texture->valid = true;
    return texture;
}

inline int textureWidth(MTexture texture) {
    if (texture == NULL || !texture->valid)
        return 0;
    return texture->img.subtex->width;
}

inline int textureHeight(MTexture texture) {
    if (texture == NULL || !texture->valid)
        return 0;
    return texture->img.subtex->height;
}

void freeTexture(MTexture texture) {
    if (texture == NULL || !texture->valid)
        return;
    texture->valid = false;
    linearFree(texture->img.subtex);
    linearFree(texture);
}

void drawTextureAt(MTexture texture, float x, float y, float scaleX, float scaleY, float angle, MColor color, float blend) {
    if (texture == NULL || !texture->valid)
        return;

    C2D_ImageTint tint;
    C2D_PlainImageTint(&tint, __builtin_bswap32(color), blend); /* RGBA8 -> ABGR8 */

    if (angle == 0) {
        C2D_DrawImageAt(texture->img, x, y, 0, &tint, scaleX, scaleY);
    } else {
        C2D_DrawImageAtRotated(texture->img, x, y, 0, angle, &tint, scaleX, scaleY);
    }
}

void drawRect(float x, float y, float w, float h, MColor color) {
    C2D_DrawRectSolid(x, y, 0, w, h, __builtin_bswap32(color)); /* RGBA8 -> ABGR8 */
}

void setDrawMode(DrawMode mode) {
    C2D_Flush();

    if (mode == DM_NORMAL) {
        C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
        C3D_StencilTest(false, GPU_ALWAYS, 0x00, 0xFF, 0x00);
        C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
    } else if (mode == DM_MODIFY_SCISSOR) {
        C3D_DepthTest(true, GPU_NEVER, 0);
        C3D_StencilTest(true, GPU_NEVER, 1, 0xFF, 0xFF);
        // clear stencil
        C3D_StencilOp(GPU_STENCIL_ZERO, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
        drawRect(0, 0, 400, 240, 0); // TODO: this is problematic
        // further setup
        C3D_StencilOp(GPU_STENCIL_REPLACE, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
        C3D_AlphaTest(true, GPU_GREATER, 0);
    } else if (mode == DM_SCISSOR || mode == DM_SCISSOR_INVERTED) {
        C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
        C3D_StencilTest(true, GPU_EQUAL, mode == DM_SCISSOR ? 1 : 0, 0xFF, 0x0);
        C3D_AlphaTest(false, GPU_ALWAYS, 0x00);
        C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_REPLACE);
    }
}

void setClearColor(MColor color) {
    clearColor = __builtin_bswap32(color); /* RGBA8 -> ABGR8 */
}

#endif
