#include "RenderLights.h"

Image playerLightBakeImage;
Image lanternLightBakeImage;
Image glowwormLightBakeImage;
Image glowwormBigLightBakeImage;

Texture playerLightBake;
Texture lanternLightBake;
Texture glowwormLightBake;
Texture glowwormBigLightBake;

// software rendering is slow -> bake light to texture
static void bakeLight(Image image, int x, int y, int r) {
    int x0 = x - r;
    int x1 = x + r;
    int y0 = y - r;
    int y1 = y + r;

    // The game's resolution is actually 200x120, the textures are all scaled up by 2.
    if (x0 < 0)
        x0 = 0;
    if (y0 < 0)
        y0 = 0;
    if (x1 > imageWidth(image))
        x1 = imageWidth(image);
    if (y1 > imageHeight(image))
        y1 = imageHeight(image);

    int xx, yy;
    for (yy = y0; yy < y1; yy++) {
        int yd = yy - y;
        yd = yd * yd;
        for (xx = x0; xx < x1; xx++) {
            int xd = xx - x;
            int dist = xd * xd + yd;
            if (dist <= r * r) {
                if (dist >= (r - (r / 8)) * (r - (r / 8))) {
                    if (xd % 2 == 0 && yd % 2 == 0) {
                        setPixel(image, xx, yy, 0x000000FF); // set pixel to be lit
                    }
                } else if (dist >= (r - (r / 4)) * (r - (r / 4))) {
                    if ((xd % 2 == 0 && yd % 2 != 0) || (xd % 2 != 0 && yd % 2 == 0)) {
                        setPixel(image, xx, yy, 0x000000FF); // set pixel to be lit
                    }
                } else {
                    setPixel(image, xx, yy, 0x000000FF); // set pixel to be lit
                }
            }
        }
    }
}

void bakeLights() {
    playerLightBakeImage = createImage(64, 64);
    lanternLightBakeImage = createImage(128, 128);
    glowwormLightBakeImage = createImage(64, 64);
    glowwormBigLightBakeImage = createImage(64, 64);

    bakeLight(playerLightBakeImage, 32, 32, 32);
    bakeLight(lanternLightBakeImage, 64, 64, 64);
    bakeLight(glowwormLightBakeImage, 12, 12, 9);
    bakeLight(glowwormBigLightBakeImage, 12, 12, 11);

    playerLightBake = createFullTexture(playerLightBakeImage);
    lanternLightBake = createFullTexture(lanternLightBakeImage);
    glowwormLightBake = createFullTexture(glowwormLightBakeImage);
    glowwormBigLightBake = createFullTexture(glowwormBigLightBakeImage);
}

void freeLightBakes() {
    freeTexture(playerLightBake);
    freeTexture(lanternLightBake);
    freeTexture(glowwormLightBake);
    freeTexture(glowwormBigLightBake);

    freeImage(playerLightBakeImage);
    freeImage(lanternLightBakeImage);
    freeImage(glowwormLightBakeImage);
    freeImage(glowwormBigLightBakeImage);
}

void renderLightsToStencil(PlayerData *pd, bool force, bool invert, bool rplayer) {
    if (force || (pd->entity.level > 1 && pd->entity.level != 5)) {
        setDrawMode(DM_MODIFY_SCISSOR);

        if (pd->activeItem->id == ITEM_LANTERN)
            renderLight(pd->entity.x, pd->entity.y, lanternLightBake);
        else if (rplayer)
            renderLight(pd->entity.x, pd->entity.y, playerLightBake);

        int i;
        for (i = 0; i < eManager.lastSlot[pd->entity.level]; ++i) {
            Entity e = eManager.entities[pd->entity.level][i];
            if (e.type == ENTITY_FURNITURE) {
                if (e.entityFurniture.itemID == ITEM_LANTERN && e.x > pd->entity.x - 160 && e.y > pd->entity.y - 125 && e.x < pd->entity.x + 160 && e.y < pd->entity.y + 125)
                    renderLight(e.x, e.y, lanternLightBake);
            } else if (e.type == ENTITY_GLOWWORM && e.x > pd->entity.x - 160 && e.y > pd->entity.y - 125 && e.x < pd->entity.x + 160 && e.y < pd->entity.y + 125) { // TODO could be made smaller becuase of smaller light radius
                if (rand() % 10 == 0)
                    continue;
                else if (rand() % 100 == 0)
                    renderLight(e.x + 20, e.y + 19, glowwormBigLightBake);
                else
                    renderLight(e.x + 20, e.y + 19, glowwormLightBake);
            }
        }

        int xo = offsetX >> 4;
        int yo = offsetY >> 4;
        int x, y;
        // added offset to render lights from lava which is offscreen
        for (x = xo - 2; x <= 13 + xo + 2; ++x) {
            for (y = yo - 2; y <= 8 + yo + 2; ++y) {
                if (getTile(pd->entity.level, x, y) == TILE_LAVA) {
                    // experimental "speedhack"
                    if (getTile(pd->entity.level, x + 1, y) == TILE_LAVA && getTile(pd->entity.level, x - 1, y) == TILE_LAVA && getTile(pd->entity.level, x, y + 1) == TILE_LAVA && getTile(pd->entity.level, x, y - 1) == TILE_LAVA) {
                        if ((x + y) % 2 == 0)
                            continue;
                    }
                    renderLight((x << 4) + 8, (y << 4) + 8, playerLightBake);
                }
            }
        }

        setDrawMode(invert ? DM_SCISSOR_INVERTED : DM_SCISSOR);
    }
}

void renderLight(int x, int y, Texture texture) {
    drawTextureAt(texture, (x - (textureWidth(texture) / 2.0f) - offsetX) * 2, (y - (textureHeight(texture) / 2.0f) - offsetY) * 2, 2, 2, 0, 0xFFFFFFFF, 0);
}

void resetStencil() {
    setDrawMode(DM_NORMAL);
}
