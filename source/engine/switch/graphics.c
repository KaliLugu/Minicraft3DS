#ifdef __SWITCH__
#include "../engine.h"

#include <raylib.h>

#define VIRTUAL_SCREEN_WIDTH 400
#define VIRTUAL_SCREEN_HEIGHT 240

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

struct _img {
    Image image;
    Texture2D texture;
    bool dirty;
};

struct _tex {
    MImage _img;
    Rectangle source;
    bool valid;
};

static Color clearColor = {0, 0, 0, 255};
static DrawMode drawMode = DM_NORMAL;

static RenderTexture2D target;
static RenderTexture2D scissorTarget;
static Shader scissorShader;
static int scissorTexLoc;
static int scissorInvLoc;

static Rectangle screenSourceRec;
static Rectangle screenDestRec;
static Vector2 screenOrigin;

static inline Color toRayColor(MColor color) {
    Color c = {(color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF};
    return c;
}
static inline MColor fromRayColor(Color color) {
    return (((MColor)color.r) << 24) | (((MColor)color.g) << 16) | (((MColor)color.b) << 8) | (((MColor)color.a));
}

void initGraphics() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minicraft");
    SetTargetFPS(60);

    const float virtualRatio = (float)SCREEN_WIDTH / (float)VIRTUAL_SCREEN_WIDTH;
    const float xOffset = 40;

    target = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    scissorTarget = LoadRenderTexture(VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);

    const char *fs =
        "#version 100                       \n"
        "precision mediump float;           \n"
        "varying vec2 fragTexCoord;         \n"
        "varying vec4 fragColor;            \n"
        "uniform sampler2D texture0;        \n"
        "uniform sampler2D scissorTex;      \n"
        "uniform int scissorInv;            \n"
        "uniform vec4 colDiffuse;           \n"
        "void main()                        \n"
        "{                                  \n"
        "    vec4 scissorColor = texture2D(scissorTex, vec2(gl_FragCoord.x / 400.0, gl_FragCoord.y / 240.0)); \n"
        "    if(scissorInv == 0 && scissorColor.r < 0.1) { discard; } \n"
        "    if(scissorInv == 1 && scissorColor.r > 0.1) { discard; } \n"
        "    vec4 texelColor = texture2D(texture0, fragTexCoord); \n"
        "    gl_FragColor = texelColor*colDiffuse*fragColor;      \n"
        "}                                  \n";
    scissorShader = LoadShaderFromMemory(0, fs);
    scissorTexLoc = GetShaderLocation(scissorShader, "scissorTex");
    scissorInvLoc = GetShaderLocation(scissorShader, "scissorInv");

    screenSourceRec = (Rectangle){0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height};
    screenDestRec = (Rectangle){-virtualRatio + xOffset, -virtualRatio, SCREEN_WIDTH + (virtualRatio * 2), SCREEN_HEIGHT + (virtualRatio * 2)};
    screenOrigin = (Vector2){0.0f, 0.0f};
}

void drawGraphics(DrawCallback callback) {
    BeginTextureMode(target);
    ClearBackground(clearColor);
    callback(0, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT);
    setDrawMode(DM_NORMAL);
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(target.texture, screenSourceRec, screenDestRec, screenOrigin, 0.0f, WHITE);
    EndDrawing();
}

void exitGraphics() {
    UnloadShader(scissorShader);
    UnloadRenderTexture(scissorTarget);
    UnloadRenderTexture(target);
    CloseWindow();
}

sInt getScreenCount() {
    return 1;
}

MImage createImage(int width, int height) {
    MImage image = malloc(sizeof(_img));
    image->image = GenImageColor(width, height, (Color){0, 0, 0, 0});
    image->texture = LoadTextureFromImage(image->image);
    image->dirty = false;
    return image;
}

MImage loadImage(char *name) {
    MImage image = malloc(sizeof(_img));
    image->image = LoadImage(name);
    image->texture = LoadTextureFromImage(image->image);
    image->dirty = false;
    return image;
}

inline int imageWidth(MImage image) {
    return image->image.width;
}

inline int imageHeight(MImage image) {
    return image->image.height;
}

void freeImage(MImage image) {
    if (image == NULL)
        return;
    UnloadImage(image->image);
    free(image);
}

MColor getPixel(MImage image, int x, int y) {
    if (image == NULL)
        return 0;

    return fromRayColor(GetImageColor(image->image, x, y));
}

void setPixel(MImage image, int x, int y, MColor color) {
    if (image == NULL)
        return;

    ImageDrawPixel(&image->image, x, y, toRayColor(color));
    image->dirty = true;
}

MTexture createTexture(MImage image, int x0, int y0, int x1, int y1) {
    MTexture texture = malloc(sizeof(_tex));
    texture->_img = image;
    texture->source = (Rectangle){x0, y0, x1 - x0, y1 - y0};
    texture->valid = true;
    return texture;
}

inline int textureWidth(MTexture texture) {
    if (texture == NULL || !texture->valid)
        return 0;
    return texture->source.width;
}

inline int textureHeight(MTexture texture) {
    if (texture == NULL || !texture->valid)
        return 0;
    return texture->source.height;
}

void freeTexture(MTexture texture) {
    if (texture == NULL || !texture->valid)
        return;
    texture->valid = false;
    free(texture);
}

void drawTextureAt(MTexture texture, float x, float y, float scaleX, float scaleY, float angle, MColor color, float blend) {
    if (texture == NULL || !texture->valid)
        return;

    if (texture->_img->dirty) {
        // "update" gpu texture
        UnloadTexture(texture->_img->texture);
        texture->_img->texture.id = 0;
        texture->_img->texture = LoadTextureFromImage(texture->_img->image);
        texture->_img->dirty = false;
    }

    Rectangle src = texture->source;
    if (scaleX < 0) {
        src.width = -src.width;
        scaleX = -scaleX;
    }
    if (scaleY < 0) {
        src.height = -src.height;
        scaleY = -scaleY;
    }

    float w = texture->source.width;
    float h = texture->source.height;
    Vector2 origin = {0.0f, 0.0f};

    w *= scaleX;
    h *= scaleY;
    Rectangle dest = {roundf(x), roundf(y), roundf(w), roundf(h)};

    DrawTexturePro(texture->_img->texture, src, dest, origin, angle, blend == 0 ? WHITE : toRayColor(color));
}

void drawRect(float x, float y, float w, float h, MColor color) {
    DrawRectangle(x, y, w, h, toRayColor(color));
}

void setDrawMode(DrawMode mode) {
    if (drawMode == DM_SCISSOR || drawMode == DM_SCISSOR_INVERTED) {
        EndShaderMode();
    }
    EndTextureMode();

    if (mode == DM_NORMAL) {
        BeginTextureMode(target);
    } else if (mode == DM_MODIFY_SCISSOR) {
        BeginTextureMode(scissorTarget);
        ClearBackground((Color){0, 0, 0, 0});
    } else if (mode == DM_SCISSOR || mode == DM_SCISSOR_INVERTED) {
        BeginTextureMode(target);
        BeginShaderMode(scissorShader);
        SetShaderValueTexture(scissorShader, scissorTexLoc, scissorTarget.texture);
        int inv = mode == DM_SCISSOR_INVERTED ? 1 : 0;
        SetShaderValue(scissorShader, scissorInvLoc, &inv, SHADER_UNIFORM_INT);
    }
    drawMode = mode;
}

void setClearColor(MColor color) {
    clearColor = toRayColor(color);
}

#endif
