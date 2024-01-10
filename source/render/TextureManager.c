#include "TextureManager.h"
#include "../Globals.h"

Texture bottomBGFull;
Texture minimap[6];
Texture font[256 / 8 * 2];
Texture tiles8[(512 / 8) * (512 / 8)];
Texture tiles16[(512 / 16) * (512 / 16)];
Texture tiles32[(512 / 32) * (512 / 32)];
Texture tiles64[(512 / 64) * (512 / 64)];
Texture playerTiles[(736 / 16) * (160 / 16)];

Color dirtColor[5];
Color grassColor;
Color sandColor;
Color waterColor[2];
Color lavaColor[2];
Color rockColor[4];
Color woodColor;
Color ironColor;
Color goldColor;
Color gemColor;
Color dungeonColor[2];
Color myceliumColor;
Color mushroomColor;
Color snowColor;
Color iceColor;

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

static void reloadTiles(Texture tiles[], int size) {
    int iHeight = imageHeight(imageIcons);
    for (int x = 0; x < 512 / size; x++) {
        for (int y = 0; y < 512 / size; y++) {
            int index = x + y * (512 / size);
            freeTexture(tiles[index]);
            tiles[index] = createTexture(imageIcons, x * size, iHeight - size - y * size, x * size + size, iHeight - size - y * size + size);
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

    int fiHeight = imageHeight(imageFont);
    for (int i = 0; i < (256 / 8) * 2; i++) {
        int x = i % (256 / 8);
        int y = i / (256 / 8);
        freeTexture(font[i]);
        font[i] = createTexture(imageFont, x * 8, fiHeight - 8 - y * 8, x * 8 + 8, fiHeight - 8 - y * 8 + 8);
    }

    reloadTiles(tiles8, 8);
    reloadTiles(tiles16, 16);
    reloadTiles(tiles32, 32);
    reloadTiles(tiles64, 64);

    int piHeight = imageHeight(imagePlayerSprites);
    for (int x = 0; x < 736 / 16; x++) {
        for (int y = 0; y < 160 / 16; y++) {
            int index = x + y * (736 / 16);
            freeTexture(playerTiles[index]);
            playerTiles[index] = createTexture(imagePlayerSprites, x * 16, piHeight - 16 - y * 16, x * 16 + 16, piHeight - 16 - y * 16 + 16);
        }
    }

    reloadColors();
}
