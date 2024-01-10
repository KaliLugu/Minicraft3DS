#include "RenderText.h"
#include "TextureManager.h"

#include "../Render.h"

static char *fontChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ      0123456789.,!?'\"-+=/\\%()<>:;    ";

void renderText(char *msg, int x, int y) {
    x -= offsetX;
    y -= offsetY;
    for (int i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char)msg[i])) - fontChars;
        if (ix >= 0)
            drawTextureAt(font + ix, (x + i * 8) * 2, y * 2, 2, 2, 0, 0xFFFFFFFF, 0);
    }
}

void renderTextCentered(char *text, int y, int width) {
    renderText(text, (width / 2 - strlen(text) * 8) / 2, y);
}

void renderTextColor(char *msg, int x, int y, Color color) {
    x -= offsetX;
    y -= offsetY;
    for (int i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char)msg[i])) - fontChars;
        if (ix >= 0)
            drawTextureAt(font + ix, (x + i * 8) * 2, y * 2, 2, 2, 0, color, 1);
    }
}

// Changes text color after the first space
void renderTextColorSpecial(char *msg, int x, int y, Color color, Color color2) {
    x -= offsetX;
    y -= offsetY;
    bool sOver = false;
    for (int i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char)msg[i])) - fontChars;
        if (msg[i] == ' ')
            sOver = true;
        if (ix >= 0) {
            if (sOver)
                drawTextureAt(font + ix, (x + i * 8) * 2, y * 2, 2, 2, 0, color2, 1);
            else
                drawTextureAt(font + ix, (x + i * 8) * 2, y * 2, 2, 2, 0, color, 1);
        }
    }
}
