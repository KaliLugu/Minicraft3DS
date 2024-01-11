#include "TextureManager.h"
#include "../Globals.h"

MTexture bottomBGFull;
MTexture minimap[6];
MTexture font[256 / 8 * 2];
MTexture tiles8[(512 / 8) * (512 / 8)];
MTexture tiles16[(512 / 16) * (512 / 16)];
MTexture tiles32[(512 / 32) * (512 / 32)];
MTexture tiles64[(512 / 64) * (512 / 64)];
MTexture playerTiles[(736 / 16) * (160 / 16)];

MColor dirtColor[5];
MColor grassColor;
MColor sandColor;
MColor waterColor[2];
MColor lavaColor[2];
MColor rockColor[4];
MColor woodColor;
MColor ironColor;
MColor goldColor;
MColor gemColor;
MColor dungeonColor[2];
MColor myceliumColor;
MColor mushroomColor;
MColor snowColor;
MColor iceColor;

static void reloadColors() {
    dirtColor[0] = getPixel(imageIcons, 16, 0);
    dirtColor[1] = getPixel(imageIcons, 16, 1);
    dirtColor[2] = getPixel(imageIcons, 16, 2);
    dirtColor[3] = getPixel(imageIcons, 16, 3);
    dirtColor[4] = getPixel(imageIcons, 16, 4);

    grassColor = getPixel(imageIcons, 17, 0);
    myceliumColor = getPixel(imageIcons, 17, 1);
    mushroomColor = getPixel(imageIcons, 17, 2);

    sandColor = getPixel(imageIcons, 18, 0);

    waterColor[0] = getPixel(imageIcons, 19, 0);
    waterColor[1] = getPixel(imageIcons, 19, 1);

    lavaColor[0] = getPixel(imageIcons, 20, 0);
    lavaColor[1] = getPixel(imageIcons, 20, 1);

    rockColor[0] = getPixel(imageIcons, 21, 0);
    rockColor[1] = getPixel(imageIcons, 21, 1);
    rockColor[2] = getPixel(imageIcons, 21, 2);
    rockColor[3] = getPixel(imageIcons, 21, 3);

    woodColor = getPixel(imageIcons, 22, 0);

    ironColor = getPixel(imageIcons, 23, 0);
    goldColor = getPixel(imageIcons, 23, 1);
    gemColor = getPixel(imageIcons, 23, 2);

    dungeonColor[0] = getPixel(imageIcons, 24, 0);
    dungeonColor[1] = getPixel(imageIcons, 24, 1);

    snowColor = getPixel(imageIcons, 25, 0);
    iceColor = getPixel(imageIcons, 25, 1);
}

static void reloadTiles(MTexture tiles[], int size) {
    for (int x = 0; x < 512 / size; x++) {
        for (int y = 0; y < 512 / size; y++) {
            int index = x + y * (512 / size);
            freeTexture(tiles[index]);
            tiles[index] = createTexture(imageIcons, x * size, y * size, x * size + size, y * size + size);
        }
    }
}

void textureManagerReload() {
    freeTexture(bottomBGFull);
    bottomBGFull = createFullTexture(imageBottombg);

    for (int i = 0; i < 6; i++) {
        freeTexture(minimap[i]);
        minimap[i] = createFullTexture(imageMinimap[i]);
    }

    for (int i = 0; i < (256 / 8) * 2; i++) {
        int x = i % (256 / 8);
        int y = i / (256 / 8);
        freeTexture(font[i]);
        font[i] = createTexture(imageFont, x * 8, y * 8, x * 8 + 8, y * 8 + 8);
    }

    reloadTiles(tiles8, 8);
    reloadTiles(tiles16, 16);
    reloadTiles(tiles32, 32);
    reloadTiles(tiles64, 64);

    for (int x = 0; x < 736 / 16; x++) {
        for (int y = 0; y < 160 / 16; y++) {
            int index = x + y * (736 / 16);
            freeTexture(playerTiles[index]);
            playerTiles[index] = createTexture(imagePlayerSprites, x * 16, y * 16, x * 16 + 16, y * 16 + 16);
        }
    }

    reloadColors();
}
