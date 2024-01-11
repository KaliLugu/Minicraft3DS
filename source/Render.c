#include "Render.h"
#include "render/RenderTiles.h"
#include "render/TextureManager.h"

extern int syncTickCount;

int offsetX, offsetY;

void renderTitle(int x, int y) {
    // MINICRAFT
    for (int xt = 0; xt < 7; xt++) {
        renderTile16(x - 26 + xt * 16, y, xt, 15, 0);
    }
    // 3DS HOMEBREW EDITION
    for (int xt = 0; xt < 9; xt++) {
        renderTile16(x - 48 + xt * 16, y + 18, xt + 7, 15, 0);
    }
}

void renderButtonIcon(sInt keyIcon, int x, int y) {
    switch (keyIcon) {
    case CIRCLEPAD:
        renderTile16(x, y, 6, 13, 0);
        break;
    case I_SL_UP:
        renderTile16(x, y, 7, 13, 0);
        break;
    case I_SL_LEFT:
        renderTile16(x, y, 8, 13, 0);
        break;
    case I_SL_DOWN:
        renderTile16(x, y, 9, 13, 0);
        break;
    case I_SL_RIGHT:
        renderTile16(x, y, 10, 13, 0);
        break;

        /* New 3DS only */
    case CSTICK:
        renderTile16(x, y, 11, 13, 0);
        break;
    case I_SR_UP:
        renderTile16(x, y, 12, 13, 0);
        break;
    case I_SR_LEFT:
        renderTile16(x, y, 13, 13, 0);
        break;
    case I_SR_DOWN:
        renderTile16(x, y, 14, 13, 0);
        break;
    case I_SR_RIGHT:
        renderTile16(x, y, 15, 13, 0);
        break;

    case I_A:
        renderTile16(x, y, 0, 14, 0);
        break;
    case I_B:
        renderTile16(x, y, 1, 14, 0);
        break;
    case I_X:
        renderTile16(x, y, 2, 14, 0);
        break;
    case I_Y:
        renderTile16(x, y, 3, 14, 0);
        break;
    case I_DP_UP:
        renderTile16(x, y, 4, 14, 0);
        break;
    case I_DP_LEFT:
        renderTile16(x, y, 5, 14, 0);
        break;
    case I_DP_DOWN:
        renderTile16(x, y, 6, 14, 0);
        break;
    case I_DP_RIGHT:
        renderTile16(x, y, 7, 14, 0);
        break;
    case I_START_PLUS:
        renderTile16(x - 8, y, 8, 14, 0);
        renderTile16(x + 8, y, 9, 14, 0);
        break;
    case I_SELECT_MINUS:
        renderTile16(x - 8, y, 10, 14, 0);
        renderTile16(x + 8, y, 11, 14, 0);
        break;
    case I_L:
        renderTile16(x, y, 12, 14, 0);
        break;
    case I_R:
        renderTile16(x, y, 13, 14, 0);
        break;

        /* New 3DS only */
    case I_ZL:
        renderTile16(x, y, 14, 14, 0);
        break;
    case I_ZR:
        renderTile16(x, y, 15, 14, 0);
        break;
    }
}

int getFrame(int a, int b, int s) {
    return (a == s) ? 0 : ((a < b - 1) ? 1 : 2);
}

void renderFrame(int x1, int y1, int x2, int y2, MColor bgColor) {
    int startX = x1;
    int startY = y1;
    drawRect((x1 << 4) + 4 - (offsetX << 1),
             (y1 << 4) + 4 - (offsetY << 1), ((x2 - x1) << 4) - 8,
             ((y2 - y1) << 4) - 8, bgColor);

    while (x1 < x2) {
        y1 = startY;
        while (y1 < y2) {
            int xp = (x1 << 3);
            int yp = (y1 << 3);
            renderTile8(xp, yp, getFrame(x1, x2, startX), 25 + getFrame(y1, y2, startY), 0);
            ++y1;
        }
        ++x1;
    }
}

void renderZoomedMap(PlayerData *pd) {
    drawRect(0, 0, 320, 240, 0x0C0C0CFF);

    int mx = pd->mapScrollX;
    int my = pd->mapScrollY;
    if (pd->mapZoomLevel == 2)
        mx = 32;
    drawTextureAt(minimap[pd->entity.level], mx, my, pd->mapZoomLevel, pd->mapZoomLevel, 0, 0xFFFFFFFF, 0); // zoomed map

    // Airwizard on zoomed map
    if (pd->entity.level == 0) {
        if (awX != 0 && awY != 0) {
            renderTile16(
                (mx + ((awX / 16) * pd->mapZoomLevel) - 16) / 2,
                (my + ((awY / 16) * pd->mapZoomLevel) - 16) / 2,
                10, 7,
                ((pd->entity.p.walkDist >> 6) & 1) == 0 ? 0 : 1);
        }
    }
    // Player on zoomed map
    // TODO: Render actual player sprite
    // TODO: Maybe also render other players?
    renderTile16(
        (mx + ((pd->entity.x / 16) * pd->mapZoomLevel) - 16) / 2,
        (my + ((pd->entity.y / 16) * pd->mapZoomLevel) - 16) / 2,
        0, 7,
        ((pd->entity.p.walkDist >> 6) & 1) == 0 ? 0 : 1);

    renderText(pd->mapText, 224, 214);     // "x2"/"x4"/"x6"
    renderTile16(142, 2, 5, 13, 0);        // Exit button
    renderTile16(126, 102, 3, 13, 0);      // Plus/Minus zoom buttons
    renderTile16(126 + 16, 102, 4, 13, 0); // Plus/Minus zoom buttons
    if (pd->mapZoomLevel < 3)
        drawRect(258, 210, 26, 20, 0x4F4F4F7F); // gray out minus button
    else if (pd->mapZoomLevel > 5)
        drawRect(284, 210, 26, 20, 0x4F4F4F7F); // gray out minus button
}

void renderHealthAndStamina(PlayerData *pd, sInt x, sInt y) {
    for (int i = 0; i < 10; ++i) {
        if (i < pd->entity.p.health)
            renderTile8(i * 8 + x, y, 21, 19, 0);
        else
            renderTile8(i * 8 + x, y, 22, 19, 0);
        if (i < pd->entity.p.stamina)
            renderTile8(i * 8 + x, y + 9, 23, 19, 0);
        else
            renderTile8(i * 8 + x, y + 9, 24, 19, 0);
    }
}

char scoreT[32];
void renderGui(PlayerData *pd) {
    // health and stamina
    renderHealthAndStamina(pd, 6, 5);

    // minimap
    drawTexture(minimap[pd->entity.level], 10, 102);

    // active item
    renderItemWithTextCentered(pd->activeItem, 160, 33);
    itoa(pd->score, scoreT, 10); // integer to base10 string
    renderText("Score:", 104, 6);
    renderText(scoreT, (248 - (strlen(scoreT) * 8)) / 2, 14);
    if (pd->entity.level == 0) {
        if (awX != 0 && awY != 0) {
            renderTile8(1 + (awX / 32), 47 + (awY / 32), 3, 27, 0); // Mini-AWizard head.
        }
    }
    // TODO: Maybe also render other players?
    renderTile8(1 + (pd->entity.x / 32), 47 + (pd->entity.y / 32), 3, 26, 0); // Mini-Player head.

    // quick select
    renderText("Select:", 90, 58);

    Inventory *inv = &(pd->inventory);
    Item *item;
    for (int i = 0; i < 8; ++i) {
        if ((inv->lastSlot) > i) {
            int xip = i % 4;
            int yip = i / 4;

            item = &inv->items[i];
            renderItemIcon(item->id, item->countLevel, 81 + xip * 21, 77 + yip * 21);
        }
    }
}

void renderPlayer(PlayerData *pd, float scale) {
    if (pd->entity.level != getLocalPlayer()->entity.level) {
        return;
    }
    if (pd->entity.p.isDead) {
        return;
    }
    int xo = pd->entity.x - 8;
    int yo = pd->entity.y - 8;

    // attack animation upwards
    if (pd->entity.p.attackTimer > 0 && pd->entity.p.dir == 1) {
        renderTile8(xo, yo - 4, 2, 20, 0);
        renderTile8(xo + 8, yo - 4, 3, 20, 0);
        renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo + 4, yo - 4);
    }

    // find basic indices
    int aIndexBig = 0;
    int aIndexSmall = 0;
    switch (pd->entity.p.dir) {
    case 0: // down
        aIndexBig = 0;
        aIndexSmall = 0;
        break;
    case 1: // up
        aIndexBig = 2;
        aIndexSmall = 1;
        break;
    case 2: // left
        aIndexBig = 7;
        aIndexSmall = 3;
        break;
    case 3: // right
        aIndexBig = 4;
        aIndexSmall = 2;
        break;
    }

    // find index offset based on walk state
    sShort walkDist = pd->entity.p.walkDist;
    uByte walkingOffset = (walkDist >> 4) % 2;
    if (pd->entity.p.dir == 2 || pd->entity.p.dir == 3) {
        walkingOffset = (walkDist >> 4) % 4;
        if (walkingOffset == 2)
            walkingOffset = 0;
        if (walkingOffset == 3)
            walkingOffset = 2;
    }

    bool swimming = isWater(pd->entity.level, pd->entity.x >> 4, pd->entity.y >> 4);

    // render water anim when swimming
    if (swimming) {
        renderTile8(xo, yo + 5, 6, 20 + ((pd->entity.p.swimTimer >> 4) & 1), 0);
        renderTile8(xo + 8, yo + 5, 7, 20 + ((pd->entity.p.swimTimer >> 4) & 1), 0);
    }

    bool accFront[] = PLAYER_SPRITE_ACCS_FRONT;

    // render the different parts
    // accs (back)
    if (aIndexSmall == 0 && !accFront[pd->sprite.accs])
        renderPlayerTile16(xo, yo, (42 + aIndexSmall), pd->sprite.accs, 0, scale);
    // legs
    if (!swimming) {
        renderPlayerTile16(xo, yo, (0 + aIndexBig + walkingOffset), pd->sprite.legs, 0, scale);
    }
    // body
    renderPlayerTile16(xo, yo, (10 + aIndexBig + walkingOffset), pd->sprite.body, 0, scale);
    // arms (normal)
    if (!(pd->entity.p.isCarrying)) {
        renderPlayerTile16(xo, yo, (20 + aIndexBig + walkingOffset), pd->sprite.arms, 0, scale);
    }
    // head
    renderPlayerTile16(xo, yo, (30 + aIndexSmall), pd->sprite.head, 0, scale);
    // eyes
    renderPlayerTile16(xo, yo, (34 + aIndexSmall), pd->sprite.eyes, 0, scale);
    // arms (carrying)
    if (pd->entity.p.isCarrying) {
        renderPlayerTile16(xo, yo, (38 + aIndexSmall), pd->sprite.arms, 0, scale);
    }
    // accs (front)
    if (aIndexSmall != 0 || accFront[pd->sprite.accs])
        renderPlayerTile16(xo, yo, (42 + aIndexSmall), pd->sprite.accs, 0, scale);

    // furniture
    if (pd->entity.p.isCarrying) {
        renderFurniture(pd->activeItem->id, xo, yo - 12);
    }

    // attack animation (other directios)
    if (pd->entity.p.attackTimer > 0) {
        switch (pd->entity.p.dir) {
        case 0: // down
            renderTile8(xo - pd->entity.p.ax, yo - pd->entity.p.ay + 12, 2, 21, 0);
            renderTile8(xo - pd->entity.p.ax + 8, yo - pd->entity.p.ay + 12, 3, 21, 0);
            renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo + 4, yo + 12);
            break;
        case 2: // left
            renderTile8(xo - pd->entity.p.ax - 4, yo - pd->entity.p.ay, 4, 20, 0);
            renderTile8(xo - pd->entity.p.ax - 4, yo - pd->entity.p.ay + 8, 4, 21, 0);
            renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo - 4, yo + 4);
            break;
        case 3: // right
            renderTile8(xo - pd->entity.p.ax + 12, yo - pd->entity.p.ay, 5, 20, 0);
            renderTile8(xo - pd->entity.p.ax + 12, yo - pd->entity.p.ay + 8, 5, 21, 0);
            renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo + 12, yo + 4);
            break;
        }
    }
}

void renderWeather(uByte level, int xScroll, int yScroll) {
    if (level == 1) {
        if (worldData.season == 3) {
            int xp = -128 + ((syncTickCount >> 2) - xScroll * 2) % 128;
            int yp = -128 + ((syncTickCount >> 1) - yScroll * 2) % 128;
            int xp2 = 0 - ((syncTickCount >> 2) + xScroll * 2) % 128;
            int yp2 = -128 + ((syncTickCount >> 1) + 64 - yScroll * 2) % 128;
            for (int xt = 0; xt < 4; ++xt) {
                for (int yt = 0; yt < 3; ++yt) {
                    renderTile64(xp + xt * 128, yp + yt * 128, 3, 0, 0);
                    renderTile64(xp2 + xt * 128, yp2 + yt * 128, 3, 0, 0);
                }
            }
        }

        if (worldData.rain) {
            int xp = -((xScroll * 2) % 128);
            int yp = -128 + ((syncTickCount << 2) - yScroll * 2) % 128;
            int xp2 = -((xScroll * 2 + 8) % 128);
            int yp2 = -128 + ((syncTickCount << 1) + 64 - yScroll * 2) % 128;
            for (int xt = 0; xt < 4; ++xt) {
                for (int yt = 0; yt < 3; ++yt) {
                    renderTile64(xp + xt * 128, yp + yt * 128, 2, 0, 0);
                    renderTile64(xp2 + xt * 128, yp2 + yt * 128, 2, 0, 0);
                }
            }
        }
    }
}

void renderDayNight(PlayerData *pd) {
    if (pd->entity.level == 1 && (worldData.daytime < 6000 || worldData.daytime > 18000)) {
        int color1 = 0x0C0C0C00;
        int color2 = 0x0C0C1000;
        int alpha1 = 0x88;
        int alpha2 = 0xDD;

        if (worldData.daytime > 5000 && worldData.daytime < 6000) {
            alpha2 = (alpha2 * (1000 - (worldData.daytime - 5000))) / 1000;
            alpha1 = (alpha1 * (1000 - (worldData.daytime - 5000))) / 1000;
        } else if (worldData.daytime > 18000 && worldData.daytime < 19000) {
            alpha1 = (alpha1 * (worldData.daytime - 18000)) / 1000;
            alpha2 = (alpha2 * (worldData.daytime - 18000)) / 1000;
        }

        color1 = color1 | alpha1;
        color2 = color2 | alpha2;

        drawRect(0, 0, 400, 240, color1);
        renderLightsToStencil(pd, true, true, false);
        drawRect(0, 0, 400, 240, color2);
        resetStencil();
    }
}

void renderFurniture(int itemID, int x, int y) {
    switch (itemID) {
    case ITEM_ANVIL:
        renderTile16(x, y, 4, 8, 0);
        break;
    case ITEM_CHEST:
        renderTile16(x, y, 5, 8, 0);
        break;
    case ITEM_OVEN:
        renderTile16(x, y, 6, 8, 0);
        break;
    case ITEM_FURNACE:
        renderTile16(x, y, 7, 8, 0);
        break;
    case ITEM_WORKBENCH:
        renderTile16(x, y, 8, 8, 0);
        break;
    case ITEM_LANTERN:
        renderTile16(x, y, 9, 8, 0);
        break;
    case ITEM_LOOM:
        renderTile16(x, y, 14, 8, 0);
        break;
    case ITEM_ENCHANTER:
        renderTile16(x, y, 15, 8, 0);
        break;
    }
}

char ertxt[20];
void renderEntity(Entity *e, int x, int y) {
    switch (e->type) {
    case ENTITY_ITEM:
        if (e->entityItem.age >= 520)
            if (e->entityItem.age / 6 % 2 == 0)
                return;
        renderItemIcon(e->entityItem.item.id, e->entityItem.item.countLevel,
                       x - 4, y - 4 - (int)e->entityItem.zz);
        break;
    case ENTITY_FURNITURE:
        renderFurniture(e->entityFurniture.itemID, x - 8, y - 8);
        break;
    case ENTITY_ZOMBIE:
        switch (e->hostile.dir) {
        case 0: // down
            renderTile16Blend(x - 8, y - 8, 4, 7, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 1: // up
            renderTile16Blend(x - 8, y - 8, 5, 7, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 2: // left
            renderTile16Blend(x - 8, y - 8, 6 + ((e->hostile.walkDist >> 4) & 1), 7, 1, e->hostile.color);
            break;
        case 3: // right
            renderTile16Blend(x - 8, y - 8, 6 + ((e->hostile.walkDist >> 4) & 1), 7, 0, e->hostile.color);
            break;
        }
        break;
    case ENTITY_SKELETON:
        switch (e->hostile.dir) {
        case 0: // down
            renderTile16Blend(x - 8, y - 8, 0, 5, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 1: // up
            renderTile16Blend(x - 8, y - 8, 1, 5, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 2: // left
            renderTile16Blend(x - 8, y - 8, 2 + ((e->hostile.walkDist >> 4) & 1), 5, 1, e->hostile.color);
            break;
        case 3: // right
            renderTile16Blend(x - 8, y - 8, 2 + ((e->hostile.walkDist >> 4) & 1), 5, 0, e->hostile.color);
            break;
        }
        break;
    case ENTITY_KNIGHT:
        switch (e->hostile.dir) {
        case 0: // down
            renderTile16Blend(x - 8, y - 8, 4, 5, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 1: // up
            renderTile16Blend(x - 8, y - 8, 5, 5, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 2: // left
            renderTile16Blend(x - 8, y - 8, 6 + ((e->hostile.walkDist >> 4) & 1), 5, 1, e->hostile.color);
            break;
        case 3: // right
            renderTile16Blend(x - 8, y - 8, 6 + ((e->hostile.walkDist >> 4) & 1), 5, 0, e->hostile.color);
            break;
        }
        break;
    case ENTITY_SLIME:
        renderTile16Blend(x - 8, y - 8 - (e->hostile.randWalkTime > 0 ? 4 : 0), 8 + (e->hostile.randWalkTime > 0 ? 1 : 0), 7, 0, e->hostile.color);
        break;
    case ENTITY_AIRWIZARD:
        e->wizard.spriteAdjust = 0;
        if (e->wizard.health < 200) {
            if (syncTickCount / 4 % 3 < 2)
                e->wizard.spriteAdjust = 1;
        } else if (e->wizard.health < 1000) {
            if (syncTickCount / 5 % 4 < 2)
                e->wizard.spriteAdjust = 1;
        }
        switch (e->wizard.dir) {
        case 0: // down
            renderTile16(x - 8, y - 8, 10, 7 + e->wizard.spriteAdjust, ((e->wizard.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 1: // up
            renderTile16(x - 8, y - 8, 11, 7 + e->wizard.spriteAdjust, ((e->wizard.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 2: // left
            renderTile16(x - 8, y - 8, 12 + ((e->wizard.walkDist >> 4) & 1), 7 + e->wizard.spriteAdjust, 1);
            break;
        case 3: // right
            renderTile16(x - 8, y - 8, 12 + ((e->wizard.walkDist >> 4) & 1), 7 + e->wizard.spriteAdjust, 0);
            break;
        }
        break;
    case ENTITY_PASSIVE:
        switch (e->passive.dir) {
        case 0: // down
            renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 0, 6, ((e->passive.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 1: // up
            renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 1, 6, ((e->passive.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 2: // left
            renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 2 + ((e->passive.walkDist >> 4) & 1), 6, 1);
            break;
        case 3: // right
            renderTile16(x - 8, y - 8, (e->passive.mtype * 4) + 2 + ((e->passive.walkDist >> 4) & 1), 6, 0);
            break;
        }
        break;
    case ENTITY_TEXTPARTICLE:
        renderTextColor(e->textParticle.text, x + 1, y - (int)e->textParticle.zz + 1, 0x000000FF);
        renderTextColor(e->textParticle.text, x, y - (int)e->textParticle.zz, e->textParticle.color);
        break;
    case ENTITY_SMASHPARTICLE:
        renderTile16(x, y, 0, 10, 0);
        break;
    case ENTITY_SPARK:
        if (e->spark.age >= 200)
            if (e->spark.age / 6 % 2 == 0)
                return;
        renderTile8Rotated(x, y, 25, 19, 0, e->spark.age * 0.0349);
        break;
    case ENTITY_DRAGON:
        switch (e->dragon.dir) {
        case 0: // down
            renderTile32(x - 16, y - 16, 0 + (e->dragon.animTimer / 4), 8, 2);
            break;
        case 1: // up
            renderTile32(x - 16, y - 16, 0 + (e->dragon.animTimer / 4), 8, 0);
            break;
        case 2: // left
            renderTile32(x - 16, y - 16, 0 + (e->dragon.animTimer / 4), 9, 1);
            break;
        case 3: // right
            renderTile32(x - 16, y - 16, 0 + (e->dragon.animTimer / 4), 9, 0);
            break;
        }
        break;
    case ENTITY_DRAGONPROJECTILE:
        if (e->dragonFire.type == 0) {
            renderTile8Rotated(x, y, 0, 40, 0, e->dragonFire.age * 0.349);
        } else {
            renderTile8(x, y, 1, 40 + (e->dragonFire.age / 10), 0);
        }
        break;
    case ENTITY_MAGIC_PILLAR:
        renderTile16(x - 8, y - 8, 1, 20, 0);
        break;
    case ENTITY_ARROW:
        if (e->arrow.age >= 200)
            if (e->arrow.age / 6 % 2 == 0)
                return;

        int abits = 0;
        int ayp = 21;
        if (e->arrow.xa < 0) {
            abits += 1;
        }
        if (e->arrow.ya < 0) {
            ayp += 1;
        }
        if (e->arrow.ya > 0) {
            ayp += 1;
            abits += 2;
        }

        switch (e->arrow.itemID) {
        case ITEM_ARROW_WOOD:
            renderTile8(x - 2, y - 2, 9, ayp, abits);
            break;
        case ITEM_ARROW_STONE:
            renderTile8(x - 2, y - 2, 10, ayp, abits);
            break;
        case ITEM_ARROW_IRON:
            renderTile8(x - 2, y - 2, 11, ayp, abits);
            break;
        case ITEM_ARROW_GOLD:
            renderTile8(x - 2, y - 2, 12, ayp, abits);
            break;
        case ITEM_ARROW_GEM:
            renderTile8(x - 2, y - 2, 13, ayp, abits);
            break;
        }
        break;
    case ENTITY_GLOWWORM:
        renderTile8(x - 4, y - 4, 28, 14, 0);
        break;
    case ENTITY_NPC:
        renderTile16(x - 8, y - 8, e->npc.type, 4, 0);
    }
}

void renderEntities(uByte level, int x, int y, EntityManager *em) {
    for (int i = 0; i < em->lastSlot[level]; ++i) {
        Entity e = em->entities[level][i];
        if (e.x > x - 200 && e.y > y - 125 && e.x < x + 200 && e.y < y + 125)
            renderEntity(&e, e.x, e.y);
    }
}

void renderItemList(Inventory *inv, int xo, int yo, int x1, int y1, int selected) {
    // If lastSlot is 0, then there are no items are in the inventory.
    bool drawCursor = true;
    if (selected < 0) {
        drawCursor = false;
        selected = 0;
    }
    int w = x1 - xo;
    int h = y1 - yo - 2;
    int i1 = inv->lastSlot;
    if (i1 > h)
        i1 = h;
    int io = selected - h / 2;
    if (io > inv->lastSlot - h)
        io = inv->lastSlot - h;
    if (io < 0)
        io = 0;

    int i;
    for (i = 0; i < i1; ++i)
        renderItemWithText(&inv->items[i + io], (1 + xo) << 3, (i + 1 + yo) << 3);

    if (drawCursor) {
        int yy = selected + 1 - io + yo;
        drawRect((xo << 4) - (offsetX << 1), (yy << 4) - (offsetY << 1) + 2, 8, 10, 0x191919FF);
        renderText(">", (xo << 3), yy << 3);
        drawRect(((xo + w) << 4) - 8 - (offsetX << 1), (yy << 4) - (offsetY << 1) + 2, 8, 10, 0x191919FF);
        renderText("<", ((xo + w) << 3) - 7, yy << 3);
    }
}

void renderRecipes(RecipeManager *r, int xo, int yo, int x1, int y1, int selected) {
    int size = r->size;
    if (size < 1)
        return;
    if (selected < 0)
        selected = 0;
    int w = x1 - xo;
    int h = y1 - yo - 2;
    int i1 = size;
    if (i1 > h)
        i1 = h;
    int io = selected - h / 2;
    if (io > size - h)
        io = size - h;
    if (io < 0)
        io = 0;

    int i, col;
    for (i = 0; i < i1; ++i) {
        int x = (1 + xo) << 3, y = (i + 1 + yo) << 3;
        renderItemIcon(r->recipes[i + io].itemResult, r->recipes[i + io].itemAmountLevel, x, y);
        if (r->recipes[i + io].canCraft)
            col = 0xFFFFFFFF;
        else
            col = 0x7F7F7FFF;

        char *name;
        if (r->recipes[i + io].itemAmountLevel == 1) {
            name = itemGetName(r->recipes[i + io].itemResult, r->recipes[i + io].itemAmountLevel);
        } else {
            name = itemGetNameWithCount(r->recipes[i + io].itemResult, r->recipes[i + io].itemAmountLevel);
        }
        char limitedName[256];
        int limitedLength = strlen(name);
        if (limitedLength > w - 3)
            limitedLength = w - 3;
        strncpy(limitedName, name, limitedLength);
        limitedName[limitedLength] = '\0';

        renderTextColor(limitedName, x + 9, y + 1, col);
    }

    int yy = selected + 1 - io + yo;
    drawRect(xo << 4, (yy << 4) + 2, 8, 10, 0x191919FF);
    renderText(">", xo << 3, yy << 3);
    drawRect(((xo + w) << 4) - 8, (yy << 4) + 2, 8, 10, 0x191919FF);
    renderText("<", ((xo + w) << 3) - 7, yy << 3);
}

void renderItemWithText(Item *item, int x, int y) {
    renderItemIcon(item->id, item->countLevel, x, y);
    if (itemIsSingle(item->id, item->countLevel))
        renderText(itemGetNameWithCount(item->id, item->countLevel), x + 9, y + 1);
    else
        renderTextColorSpecial(itemGetNameWithCount(item->id, item->countLevel), x + 9, y + 1, 0xD2D2D2FF, 0xFFFFFFFF);
}

void renderItemStuffWithText(int itemID, int itemCL, bool onlyOne, int x, int y) {
    renderItemIcon(itemID, itemCL, x, y);
    if (onlyOne)
        renderText(itemGetNameWithCount(itemID, itemCL), x + 9, y + 1);
    else
        renderTextColorSpecial(itemGetNameWithCount(itemID, itemCL), x + 9, y + 1, 0xD2D2D2FF, 0xFFFFFFFF);
}

/* For bottom screen */
void renderItemWithTextCentered(Item *item, int width, int y) {
    char *tn = itemGetNameWithCount(item->id, item->countLevel);
    int x = (width - ((strlen(tn) + 2) * 8)) / 2;

    renderItemIcon(item->id, item->countLevel, x, y);

    if (itemIsSingle(item->id, item->countLevel))
        renderText(itemGetNameWithCount(item->id, item->countLevel), x + 9, y + 1);
    else
        renderTextColorSpecial(itemGetNameWithCount(item->id, item->countLevel), x + 9, y + 1, 0xD2D2D2FF, 0xFFFFFFFF);
}

void renderItemIcon(int itemID, int countLevel, int x, int y) {
    int xd;
    int yd;
    switch (itemID) {
    // TODO: This should not be here, somehow handle it in data?
    case TOOL_MAGIC_COMPASS:
        xd = worldData.compassData[getLocalPlayer()->entity.level][0] - (getLocalPlayer()->entity.x >> 4);
        yd = worldData.compassData[getLocalPlayer()->entity.level][1] - (getLocalPlayer()->entity.y >> 4);
        if (abs(yd) > abs(xd)) {
            if (yd < 0)
                renderTile8(x, y, itemGetIconX(itemID, countLevel) + 0, itemGetIconY(itemID, countLevel), 0);
            else
                renderTile8(x, y, itemGetIconX(itemID, countLevel) + 1, itemGetIconY(itemID, countLevel), 0);
        } else {
            if (xd < 0)
                renderTile8(x, y, itemGetIconX(itemID, countLevel) + 2, itemGetIconY(itemID, countLevel), 0);
            else
                renderTile8(x, y, itemGetIconX(itemID, countLevel) + 3, itemGetIconY(itemID, countLevel), 0);
        }
        break;
    default:
        renderTile8(x, y, itemGetIconX(itemID, countLevel), itemGetIconY(itemID, countLevel), 0);
        break;
    }
}
