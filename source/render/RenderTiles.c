#include "RenderTiles.h"

#include "../Render.h"
#include "TextureManager.h"

// internal values and methods
static bool tu = false;
static bool td = false;
static bool tl = false;
static bool tr = false;

static bool tul = false;
static bool tur = false;
static bool tdl = false;
static bool tdr = false;

inline void tileRender(int xp, int yp, Texture tile, int tileSize, int bits, float angle, Color color, float blend, float scale) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
    }

    drawTextureAt(tile, xp << 1, yp << 1, scaleX * scale, scaleY * scale, angle, color, blend);
}

void renderTile8Rotated(int xp, int yp, int xTile, int yTile, int bits, float angle) {
    int index = xTile + yTile * (512 / 8);
    tileRender(xp, yp, tiles8[index], 8, bits, angle, 0xFFFFFFFF, 0, 1);
}

void renderTile16(int xp, int yp, int xTile, int yTile, int bits) {
    int index = xTile + yTile * (512 / 16);
    tileRender(xp, yp, tiles16[index], 16, bits, 0, 0xFFFFFFFF, 0, 1);
}

void renderPlayerTile16(int xp, int yp, int xTile, int yTile, int bits, float scale) {
    int index = xTile + yTile * (736 / 16);
    tileRender(xp, yp, playerTiles[index], 16, bits, 0, 0xFFFFFFFF, 0, scale);
}

void renderTile16Blend(int xp, int yp, int xTile, int yTile, int bits, Color color) {
    int index = xTile + yTile * (512 / 16);
    tileRender(xp, yp, tiles16[index], 16, bits, 0, color, 1, 1);
}

void renderTile32(int xp, int yp, int xTile, int yTile, int bits) {
    int index = xTile + yTile * (512 / 32);
    tileRender(xp, yp, tiles32[index], 32, bits, 0, 0xFFFFFFFF, 0, 1);
}

void renderTile64(int xp, int yp, int xTile, int yTile, int bits) {
    int index = xTile + yTile * (512 / 64);
    tileRender(xp, yp, tiles64[index], 64, bits, 0, 0xFFFFFFFF, 0, 1);
}

static void renderDots(int x, int y, int bits1, int bits2, int bits3, int bits4, int xTile, int yTile) {
    // another speedhack for o3DS
    if (tu && tl && tr && td) {
        renderTile16(x, y, xTile, yTile, bits1);
        return;
    }

    // "convert" to 8px tile indices
    xTile = xTile << 1;
    yTile = yTile << 1;

    if (tu && tl)
        renderTile8(x, y, xTile, yTile, bits1);
    if (tu && tr)
        renderTile8(x + 8, y, xTile + 1, yTile, bits2);
    if (td && tl)
        renderTile8(x, y + 8, xTile, yTile + 1, bits3);
    if (td && tr)
        renderTile8(x + 8, y + 8, xTile + 1, yTile + 1, bits4);
}

static void resetSurrTiles() {
    tu = false;
    td = false;
    tl = false;
    tr = false;

    tul = false;
    tur = false;
    tdl = false;
    tdr = false;
}

static void checkSurrTiles8(uByte level, int xt, int yt, int id) {
    if (getTile(level, xt, yt - 1) == id)
        tu = true;
    if (getTile(level, xt - 1, yt) == id)
        tl = true;
    if (getTile(level, xt + 1, yt) == id)
        tr = true;
    if (getTile(level, xt, yt + 1) == id)
        td = true;
    if (getTile(level, xt - 1, yt - 1) == id)
        tul = true;
    if (getTile(level, xt + 1, yt - 1) == id)
        tur = true;
    if (getTile(level, xt - 1, yt + 1) == id)
        tdl = true;
    if (getTile(level, xt + 1, yt + 1) == id)
        tdr = true;
}

static void checkSurrTiles4(uByte level, int xt, int yt, int id) {
    if (getTile(level, xt, yt - 1) == id)
        tu = true;
    if (getTile(level, xt - 1, yt) == id)
        tl = true;
    if (getTile(level, xt + 1, yt) == id)
        tr = true;
    if (getTile(level, xt, yt + 1) == id)
        td = true;
}

//"public" methods
int tData = 0;
void renderTile(int i, int d, uByte level, int x, int y) {
    int age = 0;
    switch (i) {
    case TILE_GRASS:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_GRASS);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_TREE);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_FLOWER);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_SAPLING_TREE);

        if (level == 1 && worldData.season == 3)
            renderConnectedTile4(x, y, 16, 2);
        else if (level == 1 && worldData.season == 2)
            renderConnectedTile4(x, y, 16, 1);
        else
            renderConnectedTile4(x, y, 16, 0);
        break;
    case TILE_TREE:
        renderTile(TILE_GRASS, 0, level, x, y);

        checkSurrTiles8(level, x >> 4, y >> 4, TILE_TREE);

        if (worldData.season == 2) {
            renderTile8(x, y, 44 + ((tu && tl && tul) ? 2 : 0), 12, 0);
            renderTile8(x + 8, y, 45 + ((tu && tr && tur) ? 2 : 0), 12, 0);
            renderTile8(x, y + 8, 44 + ((td && tl && tdl) ? 2 : 0), 13, 0);
            renderTile8(x + 8, y + 8, 45 + ((td && tr && tdr) ? 2 : 0), 13, 0);
        } else if (worldData.season == 3) {
            renderTile8(x, y, 44 + ((tu && tl && tul) ? 2 : 0), 14, 0);
            renderTile8(x + 8, y, 45 + ((tu && tr && tur) ? 2 : 0), 14, 0);
            renderTile8(x, y + 8, 44 + ((td && tl && tdl) ? 2 : 0), 15, 0);
            renderTile8(x + 8, y + 8, 45 + ((td && tr && tdr) ? 2 : 0), 15, 0);
        } else {
            renderTile8(x, y, 32 + ((tu && tl && tul) ? 2 : 0), 6, 0);
            renderTile8(x + 8, y, 33 + ((tu && tr && tur) ? 2 : 0), 6, 0);
            renderTile8(x, y + 8, 32 + ((td && tl && tdl) ? 2 : 0), 7, 0);
            renderTile8(x + 8, y + 8, 33 + ((td && tr && tdr) ? 2 : 0), 7, 0);
        }

        break;
    case TILE_ROCK:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_ROCK);
        if (level > 1)
            renderConnectedTile8(x, y, 16, 6);
        else
            renderConnectedTile8(x, y, 21, 4);
        break;
    case TILE_HARDROCK:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_HARDROCK);
        renderConnectedTile8(x, y, 26, 4);
        break;
    case TILE_DIRT: // render dots.
        if (level > 1)
            renderTile16(x, y, 20, 5, 0);
        else
            renderTile16(x, y, 21, 5, 0);
        break;
    case TILE_SAND:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_SAND);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_CACTUS);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_SAPLING_CACTUS);

        if (level == 1 && worldData.season == 3) {
            renderConnectedTile4(x, y, 16, 2);
        } else {
            renderConnectedTile4(x, y, 20, 0);

            if (d > 0) {
                renderTile16(x, y, 21, 3, 0);
            }
        }
        break;
    case TILE_WATER:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_WATER);
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_HOLE);
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_ICE);

        renderConnectedTile8(x, y, 16, 9);

        srand((syncTickCount + (x / 2 - y) * 4311) / 10);
        renderDots(x, y, rand() & 3, rand() & 3, rand() & 3, rand() & 3, 21, 9);
        break;
    case TILE_LAVA:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_LAVA);
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_HOLE);

        renderConnectedTile8(x, y, 16, 10);

        srand((syncTickCount + (x / 2 - y) * 4311) / 10);
        renderDots(x, y, rand() & 3, rand() & 3, rand() & 3, rand() & 3, 21, 10);
        break;
    case TILE_HOLE:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_HOLE);
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_WATER);
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_LAVA);

        renderConnectedTile8(x, y, 16, 13);
        break;
    case TILE_CACTUS:
        renderTile(TILE_SAND, 0, level, x, y);
        renderTile16(x, y, 19, 3, 0);
        break;
    case TILE_FLOWER:
        renderTile(TILE_GRASS, 0, level, x, y);
        if (level == 1 && worldData.season == 3)
            renderTile16(x, y, 20, 7, d);
        else
            renderTile16(x, y, 20, 3, d);
        break;
    case TILE_STAIRS_DOWN:
        if (level == 0)
            renderTile(TILE_CLOUD, 0, level, x, y);
        renderTile16(x, y, 16, 4, 0);
        break;
    case TILE_STAIRS_UP:
        renderTile16(x, y, 17, 4, 0);
        break;
    case TILE_IRONORE:
        renderTile16(x, y, 29, 3, 0);
        break;
    case TILE_GOLDORE:
        renderTile16(x, y, 30, 3, 0);
        break;
    case TILE_GEMORE:
        renderTile16(x, y, 31, 3, 0);
        break;
    case TILE_CLOUD:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_CLOUD);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_STAIRS_DOWN);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_CLOUDCACTUS);

        renderConnectedTile4(x, y, 20, 1);
        break;
    case TILE_CLOUDCACTUS:
        renderTile(TILE_CLOUD, 0, level, x, y);
        renderTile16(x, y, 31, 4, 0);
        break;
    case TILE_SAPLING_TREE:
        renderTile(TILE_GRASS, 0, level, x, y);
        renderTile16(x, y, 18, 3, 0);
        break;
    case TILE_SAPLING_CACTUS:
        renderTile(TILE_SAND, 0, level, x, y);
        renderTile16(x, y, 18, 3, 0);
        break;
    case TILE_FARM:
        renderTile16(x, y, 22, 3, 0);
        break;
    case TILE_WHEAT:
        age = d / 20;
        if (age > 5)
            age = 5;
        renderTile16(x, y, 23 + age, 3, 0);
        break;
    case TILE_WOOD_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_WOOD_WALL);

        renderConnectedTile4(x, y, 24, 1);
        break;
    case TILE_STONE_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_STONE_WALL);

        renderConnectedTile4(x, y, 16, 5);
        break;
    case TILE_IRON_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_IRON_WALL);

        renderConnectedTile4(x, y, 28, 1);
        break;
    case TILE_GOLD_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_GOLD_WALL);

        renderConnectedTile4(x, y, 28, 0);
        break;
    case TILE_GEM_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_GEM_WALL);

        renderConnectedTile4(x, y, 20, 2);
        break;
    case TILE_DUNGEON_WALL:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_DUNGEON_WALL);

        renderConnectedTile8(x, y, 24, 2);
        break;
    case TILE_DUNGEON_FLOOR:
        renderTile16(x, y, 29 + d, 2, 0);
        break;
    case TILE_DUNGEON_ENTRANCE:
        renderTile16(x, y, 22 + (level == 5 ? 1 : 0), 5, 0);
        break;
    case TILE_MAGIC_BARRIER:
        renderTile(TILE_DUNGEON_FLOOR, 0, level, x, y);
        renderTile16(x, y, 20, 4, d);

        // draw remaining pillar count
        PlayerData *lp = getLocalPlayer();
        if ((lp->entity.x - (x + 8)) * (lp->entity.x - (x + 8)) + (lp->entity.y - (y + 8)) * (lp->entity.y - (y + 8)) <= 24 * 24) {
            int data = 0;
            int i = 0;
            for (i = 0; i < eManager.lastSlot[level]; ++i) {
                Entity *e = &eManager.entities[level][i];

                if (e->type == ENTITY_MAGIC_PILLAR) {
                    ++data;
                }
            }

            char currentCount[3];
            sprintf(currentCount, "%d", data);

            renderTextColor(currentCount, x + 4 + 1, y + 4 + 1, dungeonColor[1]);
            renderTextColor(currentCount, x + 4, y + 4, dungeonColor[0]);
        }

        break;
    case TILE_BOOKSHELVES:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_BOOKSHELVES);

        renderConnectedTile4(x, y, 24, 5 + d);
        break;
    case TILE_WOOD_FLOOR:
        renderTile16(x, y, 21, 6, 0);
        break;
    case TILE_MYCELIUM:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_MYCELIUM);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_MUSHROOM_BROWN);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_MUSHROOM_RED);

        if (level == 1 && worldData.season == 3)
            renderConnectedTile4(x, y, 16, 7);
        else
            renderConnectedTile4(x, y, 28, 5);
        break;
    case TILE_MUSHROOM_BROWN:
        renderTile(TILE_MYCELIUM, 0, level, x, y);
        renderTile16(x, y, 28 + (d & 0x1), 6, 0);
        break;
    case TILE_MUSHROOM_RED:
        renderTile(TILE_MYCELIUM, 0, level, x, y);
        renderTile16(x, y, 30 + (d & 0x1), 6, 0);
        break;
    case TILE_ICE:
        renderTile(TILE_WATER, 0, level, x, y);
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_ICE);

        renderConnectedTile8(x, y, 16, 12);
        break;
    }

    resetSurrTiles();
}

void renderConnectedTile4(int x, int y, int xTile, int yTile) {
    // render complete tile in one piece to reduce strain(added for o3DS)
    if (tl && tr && tu && td) {
        renderTile16(x, y, xTile + 3, yTile, 0);
        return;
    }

    // "convert" to 8px tile indices
    xTile = xTile << 1;
    yTile = yTile << 1;

    int l = (tl ? 2 : 0);
    int r = (tr ? 2 : 0);
    int u = (tu ? 4 : 0);
    int d = (td ? 4 : 0);

    renderTile8(x, y, xTile + l + u, yTile, 0);
    renderTile8(x + 8, y, xTile + 1 + r + u, yTile, 0);
    renderTile8(x, y + 8, xTile + l + d, yTile + 1, 0);
    renderTile8(x + 8, y + 8, xTile + 1 + r + d, yTile + 1, 0);
}

void renderConnectedTile8(int x, int y, int xTile, int yTile) {
    // render complete tile in one piece to reduce strain(added for o3DS)
    if (tl && tr && tu && td && tul && tur && tdl && tdr) {
        renderTile16(x, y, xTile + 4, yTile, 0);
        return;
    }

    // "convert" to 8px tile indices
    xTile = xTile << 1;
    yTile = yTile << 1;

    int l = (tl ? 2 : 0);
    int r = (tr ? 2 : 0);
    int u = (tu ? 4 : 0);
    int d = (td ? 4 : 0);

    renderTile8(x, y, xTile + l + u + ((tl && tu && tul) ? 2 : 0), yTile, 0);
    renderTile8(x + 8, y, xTile + 1 + r + u + ((tr && tu && tur) ? 2 : 0), yTile, 0);
    renderTile8(x, y + 8, xTile + l + d + ((tl && td && tdl) ? 2 : 0), yTile + 1, 0);
    renderTile8(x + 8, y + 8, xTile + 1 + r + d + ((tr && td && tdr) ? 2 : 0), yTile + 1, 0);
}

void renderBackground(uByte level, int xScroll, int yScroll) {
    if (level == 0) {
        drawTextureAt(minimap[1], (-xScroll / 3) - 256, (-yScroll / 3) - 32, 12.5, 7.5, 0, 0xFFFFFFFF, 0);
        drawRect(0, 0, 400, 240, 0xAFDFDFDF);
    } else if (level == 5) {
        drawRect(0, 0, 400, 240, dungeonColor[1]);
    } else {
        drawRect(0, 0, 400, 240, dirtColor[level]); // dirt color
    }

    int xo = xScroll >> 4;
    int yo = yScroll >> 4;
    int x, y;
    for (x = xo; x <= 13 + xo; ++x) {
        for (y = yo; y <= 8 + yo; ++y) {
            renderTile(getTile(level, x, y), getData(level, x, y), level, x << 4, y << 4);
        }
    }
}
