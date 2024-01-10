#include "EditorTiles.h"

#include "../Globals.h"
#include "../Ingame.h"
#include "../Render.h"

#define EDITOR_MODE_PAINT 0
#define EDITOR_MODE_PICK 1
#define EDITOR_MODE_SCROLL 2
#define EDITOR_MODE_CHOOSE 3

int editorX;
int editorY;
int editorLevel;

int editorMode;

bool editorTouchDragging;
s16 editorTouchXLast;
s16 editorTouchYLast;

int editorTile;

int editorChooseOffset;

void editorTilesInit() {
    editorX = 0;
    editorY = 0;
    editorLevel = 1;

    editorMode = EDITOR_MODE_SCROLL;

    editorTouchDragging = false;
    editorTouchXLast = 0;
    editorTouchYLast = 0;

    editorTile = 0;

    editorChooseOffset = 0;
}

void editorTilesTick() {
    // update mode (only when not currently drawing/dragging/...)
    if (localInputs.k_touch.px <= 0 && localInputs.k_touch.py <= 0) {
        editorMode = EDITOR_MODE_SCROLL;
        if (localInputs.k_up.down) {
            editorMode = EDITOR_MODE_PAINT;
        } else if (localInputs.k_left.down) {
            editorMode = EDITOR_MODE_PICK;
        } else if (localInputs.k_right.down) {
            editorMode = EDITOR_MODE_CHOOSE;
        }

        if (localInputs.k_accept.clicked) {
            if (editorLevel > 0)
                editorLevel--;
        }
        if (localInputs.k_decline.clicked) {
            if (editorLevel < 4)
                editorLevel++;
        }
    }

    // painting tiles
    if (editorMode == EDITOR_MODE_PAINT) {
        if (localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            int tilePosX = (editorX + localInputs.k_touch.px / 2) >> 4;
            int tilePosY = (editorY + localInputs.k_touch.py / 2) >> 4;

            setTile(editorTile, editorLevel, tilePosX, tilePosY);
        }
        // picking tiles
    } else if (editorMode == EDITOR_MODE_PICK) {
        if (localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            int tilePosX = (editorX + localInputs.k_touch.px / 2) >> 4;
            int tilePosY = (editorY + localInputs.k_touch.py / 2) >> 4;

            editorTile = getTile(editorLevel, tilePosX, tilePosY);
        }
        // dragging the map
    } else if (editorMode == EDITOR_MODE_SCROLL) {
        if (localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            if (editorTouchDragging) {
                s16 moveX = (editorTouchXLast - localInputs.k_touch.px) / 2;
                s16 moveY = (editorTouchYLast - localInputs.k_touch.py) / 2;

                editorX += moveX;
                editorY += moveY;

                if (editorX < 0)
                    editorX = 0;
                if (editorX > 128 * 16 - 320)
                    editorX = 128 * 16 - 320;
                if (editorY < 0)
                    editorY = 0;
                if (editorY > 128 * 16 - 240)
                    editorY = 128 * 16 - 240;
            } else {
                editorTouchDragging = true;
            }
            editorTouchXLast = localInputs.k_touch.px;
            editorTouchYLast = localInputs.k_touch.py;
        } else {
            editorTouchDragging = false;
        }
        // choosing tiles from all possible
    } else if (editorMode == EDITOR_MODE_CHOOSE) {
        if (localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            if (editorTouchDragging) {
                s16 moveY = (editorTouchYLast - localInputs.k_touch.py) / 2;

                editorChooseOffset += moveY;

                if (editorChooseOffset < 0)
                    editorChooseOffset = 0;
                if (editorChooseOffset > (MAX_TILE_ID_USED + 5) / 6 * 24 - (240 / 2 - 24))
                    editorChooseOffset = (MAX_TILE_ID_USED + 5) / 6 * 24 - (240 / 2 - 24);
            } else {
                if (localInputs.k_touch.px > (24 * 2) * 6) {
                    editorTouchDragging = true;
                } else {
                    int touchTile = (localInputs.k_touch.px / (24 * 2)) + ((localInputs.k_touch.py + editorChooseOffset * 2) / (24 * 2)) * 6;
                    if (touchTile >= 0 && touchTile <= MAX_TILE_ID_USED) {
                        editorTile = touchTile;
                    }
                }
            }
            editorTouchXLast = localInputs.k_touch.px;
            editorTouchYLast = localInputs.k_touch.py;
        } else {
            editorTouchDragging = false;
        }
    }
}

// abuses dungeon level for rendering
void editorRenderTile(int id, int data, int x, int y) {
    int xt, yt;

    for (xt = 0; xt < 3; xt++) {
        for (yt = 0; yt < 3; yt++) {
            setTile(TILE_NULL, 5, xt, yt);
        }
    }
    setTile(id, 5, 1, 1);
    setData(data, 5, 1, 1);
    offsetX = -x;
    offsetY = -y;
    renderTile(id, data, 5, 1, 1);
    offsetX = 0;
    offsetY = 0;
}

void editorTilesRenderTop(int screen, int width, int height) {
    // render current tile
    renderFrame(0, 4, 10, 15, 0x1010FFFF);

    renderTextColor("Cur Tile", 40 - (8 * 8) / 2 + 1, 40 + 1, 0x000000FF);
    renderTextColor("Cur Tile", 40 - (8 * 8) / 2, 40, 0xFFFFFFFF);

    editorRenderTile(editorTile, 0, 32, 48);

    renderTextColor(tileGetName(editorTile), 40 - (strlen(tileGetName(editorTile)) * 8) / 2 + 1, 66 + 1, 0x000000FF);
    renderTextColor(tileGetName(editorTile), 40 - (strlen(tileGetName(editorTile)) * 8) / 2, 66, 0xFFFFFFFF);

    // render current level number
    char sbuffer[10];
    sprintf(sbuffer, "Level: %i", editorLevel);
    renderTextColor(sbuffer, 4 + 1, 79 + 1, 0x000000FF);
    renderTextColor(sbuffer, 4, 79, 0xFFFFFFFF);

    renderTextColor("   up", 4 + 1, 91 + 1, 0x000000FF);
    renderTextColor("   up", 4, 91, 0xFFFFFFFF);
    renderTextColor("   down", 4 + 1, 104 + 1, 0x000000FF);
    renderTextColor("   down", 4, 104, 0xFFFFFFFF);

    renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 5, 91 - 4);
    renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 5, 104 - 4);

    // draw dp with available modes
    renderFrame(10, 4, 25, 15, 0x1010FFFF);
    int dpxpos = 140;
    int dpypos = 76;

    drawRect(dpxpos * 2 - 32 / 2, dpypos * 2 - 32 * 3 / 2, 32, 32 * 3, 0x606060FF);
    drawRect(dpxpos * 2 - 32 * 3 / 2, dpypos * 2 - 32 / 2, 32 * 3, 32, 0x606060FF);

    if (editorMode == EDITOR_MODE_SCROLL) {
    } else if (editorMode == EDITOR_MODE_PICK)
        drawRect(dpxpos * 2 - 32 * 3 / 2, dpypos * 2 - 32 / 2, 32, 32, 0x909090FF);
    else if (editorMode == EDITOR_MODE_PAINT)
        drawRect(dpxpos * 2 - 32 / 2, dpypos * 2 - 32 * 3 / 2, 32, 32, 0x909090FF);
    else if (editorMode == EDITOR_MODE_CHOOSE)
        drawRect(dpxpos * 2 + 32 / 2, dpypos * 2 - 32 / 2, 32, 32, 0x909090FF);
    else
        drawRect(dpxpos * 2 - 32 / 2, dpypos * 2 + 32 / 2, 32, 32, 0x909090FF);

    // draw texts for modes
    renderTextColor("Scroll", dpxpos - (6 * 8) / 2 + 1, dpypos - 3 + 1, 0x000000FF);
    renderTextColor("Scroll", dpxpos - (6 * 8) / 2, dpypos - 3, 0x404040FF);

    renderTextColor("Pick", dpxpos - 25 - (4 * 8) + 1, dpypos - 3 + 1, 0x000000FF);
    renderTextColor("Pick", dpxpos - 25 - (4 * 8), dpypos - 3, 0x404040FF);

    renderTextColor("Paint", dpxpos - (5 * 8) / 2 + 1, dpypos - 34 + 1, 0x000000FF);
    renderTextColor("Paint", dpxpos - (5 * 8) / 2, dpypos - 34, 0x404040FF);

    renderTextColor("Tile", dpxpos + 25 + 1, dpypos - 3 + 1, 0x000000FF);
    renderTextColor("Tile", dpxpos + 25, dpypos - 3, 0x404040FF);

    renderTextColor("Tests", dpxpos - (5 * 8) / 2 + 1, dpypos + 27 + 1, 0x000000FF);
    renderTextColor("Tests", dpxpos - (5 * 8) / 2, dpypos + 27, 0x404040FF);

    if (editorMode == EDITOR_MODE_SCROLL)
        renderTextColor("Scroll", dpxpos - (6 * 8) / 2, dpypos - 3, 0xFFFFFFFF);
    else if (editorMode == EDITOR_MODE_PICK)
        renderTextColor("Pick", dpxpos - 25 - (4 * 8), dpypos - 3, 0xFFFFFFFF);
    else if (editorMode == EDITOR_MODE_PAINT)
        renderTextColor("Paint", dpxpos - (5 * 8) / 2, dpypos - 34, 0xFFFFFFFF);
    else if (editorMode == EDITOR_MODE_CHOOSE)
        renderTextColor("Tile", dpxpos + 25, dpypos - 3, 0xFFFFFFFF);
    else {
    }
}

void editorTilesRenderBottom(int screen, int width, int height) {
    if (editorMode == EDITOR_MODE_PAINT || editorMode == EDITOR_MODE_PICK || editorMode == EDITOR_MODE_SCROLL) {
        // render map
        offsetX = editorX;
        offsetY = editorY;
        renderBackground(editorLevel, editorX, editorY);
        offsetX = 0;
        offsetY = 0;
    } else if (editorMode == EDITOR_MODE_CHOOSE) {
        int t;

        for (t = 0; t <= MAX_TILE_ID_USED; t++) {
            int xp = (t % 6);
            int yp = (t / 6);

            offsetY = editorChooseOffset;
            renderFrame(xp * 3, yp * 3, xp * 3 + 3, yp * 3 + 3, 0x1010FFFF);
            offsetY = 0;

            editorRenderTile(t, 0, xp * 24 + 3, yp * 24 + 3 - editorChooseOffset);
        }
    }
}
