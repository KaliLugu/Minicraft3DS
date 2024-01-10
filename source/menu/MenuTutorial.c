#include "MenuTutorial.h"

#include "../Globals.h"
#include "../Menu.h"
#include "../Render.h"
#include "../render/TextureManager.h"

uByte pageNum = 0;
uByte maxPageNum = 6;

char pageText[18];

void menuRenderTilePit(int x, int y, int xt, int yt) {
    renderTile16(x + 8, y + 8, xt / 2 + 3, yt / 2, 0);

    renderTile8(x, y, xt, yt, 0);
    renderTile8(x + 24, y, xt + 1, yt, 0);
    renderTile8(x, y + 24, xt, yt + 1, 0);
    renderTile8(x + 24, y + 24, xt + 1, yt + 1, 0);

    renderTile8(x + 8, y, xt + 2, yt, 0);
    renderTile8(x + 16, y, xt + 3, yt, 0);
    renderTile8(x + 8, y + 24, xt + 2, yt + 1, 0);
    renderTile8(x + 16, y + 24, xt + 3, yt + 1, 0);

    renderTile8(x, y + 8, xt + 4, yt, 0);
    renderTile8(x, y + 16, xt + 4, yt, 0);
    renderTile8(x + 24, y + 8, xt + 5, yt, 0);
    renderTile8(x + 24, y + 16, xt + 5, yt, 0);
}

void menuTutorialInit() {
    sprintf(pageText, "Page: %d/%d", pageNum + 1, maxPageNum + 1);
}

void menuTutorialTick() {
    if (localInputs.k_decline.clicked) {
        currentSelection = 2;
        currentMenu = MENU_TITLE;
    }
    if (localInputs.k_menuNext.clicked) {
        if (pageNum < maxPageNum) {
            ++pageNum;
            sprintf(pageText, "Page: %d/%d", pageNum + 1, maxPageNum + 1);
        }
    }
    if (localInputs.k_menuPrev.clicked) {
        if (pageNum > 0) {
            --pageNum;
            sprintf(pageText, "Page: %d/%d", pageNum + 1, maxPageNum + 1);
        }
    }
}

sInt biasedCirclePad(sInt in) {
    if (in & I_SL_UP)
        return I_SL_UP;
    else if (in & I_SL_DOWN)
        return I_SL_DOWN;
    else if (in & I_SL_LEFT)
        return I_SL_LEFT;
    else if (in & I_SL_RIGHT)
        return I_SL_RIGHT;
    else
        return (in & -in);
}

sInt biasedMenuXY(sInt in) {
    if (in & I_X)
        return I_X;
    else if (in & I_Y)
        return I_Y;
    else
        return (in & -in);
}

void menuTutorialRender(int screen, int width, int height) {
    /* Top Screen */
    if (screen == 0) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        renderTextColor("How to Play", (200 - 11 * 8) / 2, 6, 0xAFAF00FF);
        switch (pageNum) {
        case 0: // Moving the character
            renderTextColor("Movement", (200 - 8 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Press   to move up", 45, width);
            renderButtonIcon(biasedCirclePad(localInputs.k_up.input), 72, 40);
            renderTextCentered("Press   to move down", 60, width);
            renderButtonIcon(biasedCirclePad(localInputs.k_down.input), 64, 55);
            renderTextCentered("Press   to move left", 75, width);
            renderButtonIcon(biasedCirclePad(localInputs.k_left.input), 64, 70);
            renderTextCentered("Press   to move right", 90, width);
            renderButtonIcon(biasedCirclePad(localInputs.k_right.input), 60, 85);
            break;
        case 1: // Attacking
            renderTextColor("Attacking", (200 - 9 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Press   to Attack", 40, width);
            renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 74, 35);
            renderTextCentered("Press   to Use items", 55, width);
            renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 62, 50);

            renderTextCentered("The axe cuts down trees", 70, width);
            renderTextCentered("The sword hurts enemies", 80, width);
            renderTextCentered("The shovel digs ground", 90, width);
            renderTextCentered("The pickaxe mines rocks", 100, width);
            break;
        case 2: // Inventory
            renderTextColor("Inventory", (200 - 9 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Press   to open the menu", 40, width);
            renderButtonIcon(biasedMenuXY(localInputs.k_menu.input), 47, 34);
            renderTextCentered("Press   to scroll up", 55, width);
            renderButtonIcon(biasedCirclePad(localInputs.k_up.input), 63, 50);
            renderTextCentered("Press   to scroll down", 70, width);
            renderButtonIcon(biasedCirclePad(localInputs.k_down.input), 55, 65);
            renderTextCentered("Press   to select an item", 85, width);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 44, 80);
            renderTextCentered("Press   to close the menu", 100, width);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 44, 95);
            break;
        case 3: // Furniture
            renderTextColor("Furniture", (200 - 9 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Use to craft/store items", 37, width);
            renderTextCentered("Press   to open the menu", 50, width);
            renderButtonIcon(biasedMenuXY(localInputs.k_menu.input), 47, 45);
            renderTextCentered("while infront of one", 58, width);
            renderTextCentered("Use the lantern to light", 72, width);
            renderTextCentered("up underground areas", 80, width);
            renderTextCentered("Use the power glove to", 92, width);
            renderTextCentered("pick up furniture", 100, width);
            break;
        case 4: // Crafting
            renderTextColor("Crafting", (200 - 8 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Create items and tools", 37, width);
            renderTextCentered("Go up to a furniture and", 52, width);
            renderTextCentered("Press   to open the menu", 65, width);
            renderButtonIcon(biasedMenuXY(localInputs.k_menu.input), 47, 60);
            renderTextCentered("Gather the required materials", 77, width);
            renderTextCentered("then press   to craft it", 90, width);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 89, 87);
            break;
        case 5: // Farming
            renderTextColor("Farming", (200 - 7 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Grow wheat to make bread", 37, width);
            renderTextCentered("Dig grass to gather seeds", 47, width);
            renderTextCentered("Use a hoe to till ground", 57, width);
            renderTextCentered("Harvest wheat when yellow", 67, width);
            renderTextCentered("Use an oven to bake bread", 77, width);
            renderTextCentered("4 wheat makes one bread", 87, width);
            break;
        case 6: // Mining
            renderTextColor("Mining", (200 - 6 * 8) / 2, 20, 0xBF7F00FF);
            renderTextCentered("Use a pickaxe for mining", 37, width);
            renderTextCentered("Mine rocks for stone", 47, width);
            renderTextCentered("Mine iron ore for iron", 57, width);
            renderTextCentered("Mine gold ore for gold", 67, width);
            renderTextCentered("Mine gem ore to get gems", 77, width);
            renderTextCentered("4 ore and 1 coal make an", 95, width);
            renderTextCentered("ingot inside a furnace", 105, width);
            break;
        }
    }

    /* Bottom Screen */
    if (screen == 10) {
        drawRect(0, 0, width, height, 0x0C0C0CFF);

        switch (pageNum) {
        case 0:                            // Moving the character
            renderTile16(30, 56, 1, 7, 0); // Player up
            renderButtonIcon(biasedCirclePad(localInputs.k_up.input), 30, 40);
            renderTile16(60, 56, 0, 7, 0); // Player down
            renderButtonIcon(biasedCirclePad(localInputs.k_down.input), 60, 40);
            renderTile16(90, 56, 3, 7, 1); // Player left
            renderButtonIcon(biasedCirclePad(localInputs.k_left.input), 90, 40);
            renderTile16(120, 56, 3, 7, 0); // Player right
            renderButtonIcon(biasedCirclePad(localInputs.k_right.input), 120, 40);
            break;
        case 1:                            // Attacking
            renderTile16(60, 56, 0, 7, 0); // Player-down
            renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 80, 56);
            renderTile8(60, 68, 2, 21, 0); // Slash (Down)
            renderTile8(68, 68, 3, 21, 0); // Slash (Down)

            menuRenderTilePit(12, 20, 32, 0);                 // grass pit
            renderTile16(12 + 8, 20 + 4, 16, 3, 0);           // Tree
            renderTile8(12 + 9, 20 + 14, 2, 20, 0);           // Slash (Up)
            renderTile8(12 + 9 + 8, 20 + 14, 3, 20, 0);       // Slash (Up)
            renderItemIcon(TOOL_AXE, 1, 12 + 9 + 4, 20 + 14); // Axe
            renderTile16(12 + 9, 20 + 18, 1, 7, 0);           // Player-up

            menuRenderTilePit(122, 62, 40, 0);       // sand pit
            renderTile16(130, 70, 16, 13, 0);        // hole
            renderTile16(116, 70, 3, 7, 0);          // Player-right
            renderItemIcon(ITEM_SAND, 1, 136, 76);   // Sand Item
            renderTile8(128, 70, 5, 20, 0);          // Slash (Right)
            renderTile8(128, 70 + 8, 5, 21, 0);      // Slash (Right)
            renderItemIcon(TOOL_SHOVEL, 2, 130, 74); // Shovel
            break;
        case 2: // Inventory
            renderFrame(4, 4, 20, 11, 0x1010FFFF);
            renderItemStuffWithText(ITEM_APPLE, 5, false, 40, 39);
            renderItemStuffWithText(ITEM_SLIME, 11, false, 40, 47);
            renderItemStuffWithText(TOOL_SWORD, 4, true, 40, 55);
            renderItemStuffWithText(ITEM_IRONORE, 3, false, 40, 63);
            renderItemStuffWithText(ITEM_IRONINGOT, 11, false, 40, 71);
            drawRect(64, 110, 8, 12, 0x000000FF);
            renderText(">", 32, 55);
            renderButtonIcon(biasedCirclePad(localInputs.k_up.input), 11, 32);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 11, 52);
            renderButtonIcon(biasedCirclePad(localInputs.k_down.input), 11, 72);
            break;
        case 3: // Furniture
            drawRect(64, 48, 192, 32, grassColor);
            for (int f = 0; f < 6; f++) {
                renderTile16(32 + f * 16, 24, 4 + f, 8, 0); // Furniture entities
            }

            renderFurniture(ITEM_WORKBENCH, 50, 60);
            renderTile16(50, 46, 0, 7, 0);            // Player-down
            renderTile8(50, 58, 2, 21, 0);            // Slash (Down)
            renderTile8(58, 58, 3, 21, 0);            // Slash (Down)
            renderItemIcon(ITEM_POWGLOVE, 0, 54, 58); // Power glove

            renderTile16(92, 56, 0, 8, 0); // Player(Carrying)
            renderTile16(92, 44, 8, 8, 0); // Workbench
            break;
        case 4: // Crafting
            renderFrame(11, 3, 19, 6, 0x1010FFFF);
            renderTextColor("Have", 92 + 1, 22 + 1, 0x000000FF);
            renderTextColor("Have", 92, 22, 0xE2E26FFF);
            renderFrame(11, 7, 19, 12, 0x1010FFFF);
            renderTextColor("Cost", 92 + 1, 54 + 1, 0x000000FF);
            renderTextColor("Cost", 92, 54, 0xE2E26FFF);
            renderFrame(1, 3, 10, 12, 0x1010FFFF);
            renderTextColor("Crafting", 12 + 1, 22 + 1, 0x000000FF);
            renderTextColor("Crafting", 12, 22, 0xE2E26FFF);

            renderItemStuffWithText(TOOL_AXE, 0, true, 14, 32);
            renderItemIcon(TOOL_AXE, 0, 94, 32);
            renderText("0", 103, 33);
            renderItemIcon(ITEM_WOOD, 0, 94, 64);
            renderText("16/5", 103, 65);
            break;
        case 5: // Farming (Bottom screen)
            for (int w = 0; w < 7; w++) {
                renderTile16(24 + w * 16, 16, 22 + w, 3, 0); // Wheat Stages
            }

            renderTile16(20, 40, 22, 3, 0);        // Farm Tile
            renderTile16(36, 40, 28, 3, 0);        // Wheat Tile
            renderTile16(52, 40, 28, 3, 0);        // Wheat Tile
            renderTile16(20, 54, 1, 7, 0);         // Player (Up)
            renderTile8(20, 50, 2, 20, 0);         // Slash (Up)
            renderTile8(28, 50, 3, 20, 0);         // Slash (Up)
            renderItemIcon(ITEM_SEEDS, 1, 19, 45); // Seeds
            renderItemIcon(ITEM_WHEAT, 1, 26, 39); // Wheat1
            renderItemIcon(ITEM_WHEAT, 1, 29, 44); // Wheat2

            renderTile16(72, 40, 22, 3, 0);        // Farm Tile
            renderTile16(72 + 16, 40, 23, 3, 0);   // Seeded Wheat Tile
            renderTile16(88, 54, 1, 7, 0);         // Player (Up)
            renderTile8(88, 50, 2, 20, 0);         // Slash (Up)
            renderTile8(96, 50, 3, 20, 0);         // Slash (Up)
            renderItemIcon(ITEM_SEEDS, 1, 92, 48); // Seeds

            drawRect(216, 80, 32, 32, dirtColor[1]); // Dirt color for grass
            renderTile16(108, 40, 16, 0, 0);         // Grass
            renderTile16(124, 40, 22, 3, 0);         // Farm Tile
            renderTile16(124, 54, 1, 7, 0);          // Player (Up)
            renderTile8(124, 50, 2, 20, 0);          // Slash (Up)
            renderTile8(132, 50, 3, 20, 0);          // Slash (Up)
            renderItemIcon(TOOL_HOE, 4, 128, 48);    // Gem Hoe

            drawRect(112, 156, 32, 32, dirtColor[1]); // Dirt color for grass
            renderTile16(56, 78, 16, 0, 0);           // Grass
            drawRect(80, 156, 32, 32, dirtColor[1]);  // Dirt color
            renderTile16(40, 78, 21, 5, 0);           // Dirt Dots
            renderItemIcon(ITEM_SEEDS, 1, 44, 82);    // Seeds

            renderTile16(24, 78, 3, 7, 0);          // Player (Right)
            renderTile8(36, 78, 5, 20, 0);          // Slash (Right)
            renderTile8(36, 78 + 8, 5, 21, 0);      // Slash (Right)
            renderItemIcon(TOOL_SHOVEL, 4, 38, 82); // Gem Shovel

            renderItemIcon(ITEM_WHEAT, 1, 82, 78); // Wheat
            renderItemIcon(ITEM_WHEAT, 1, 90, 78); // Wheat
            renderItemIcon(ITEM_WHEAT, 1, 82, 86); // Wheat
            renderItemIcon(ITEM_WHEAT, 1, 90, 86); // Wheat
            renderText(">", 101, 82);
            renderTile16(108, 76, 6, 8, 0); // Oven
            renderText(">", 123, 82);
            renderItemIcon(ITEM_BREAD, 1, 132, 82); // Bread
            break;
        case 6:                                      // Mining
            renderTile16(23, 32, 29, 3, 0);          // iron ore
            renderTile16(23, 52, 30, 3, 0);          // gold ore
            renderTile16(23, 72, 31, 3, 0);          // gem ore
            renderItemIcon(ITEM_IRONORE, 1, 41, 38); // Iron ore item
            renderItemIcon(ITEM_GOLDORE, 1, 41, 58); // Gold ore item
            renderItemIcon(ITEM_GEM, 1, 41, 78);     // Gem item
            renderText(">", 52, 37);
            renderText(">", 52, 57);
            renderText(">", 52, 77);
            renderTile16(60, 32, 7, 8, 0);  // Furnace
            renderTile16(60, 52, 7, 8, 0);  // Furnace
            renderTile16(60, 72, 15, 8, 0); // Enchanter
            renderText(">", 80, 37);
            renderText(">", 80, 57);
            renderText(">", 80, 77);
            renderItemIcon(ITEM_IRONINGOT, 1, 88, 36); // Iron ingot item
            renderItemIcon(ITEM_GOLDINGOT, 1, 88, 56); // Gold ingot item
            renderItemIcon(TOOL_PICKAXE, 4, 88, 76);   // Gem Pickaxe
            renderText(">", 100, 37);
            renderText(">", 100, 57);
            renderTile16(106, 32, 4, 8, 0); // Anvil
            renderTile16(106, 52, 4, 8, 0); // Anvil
            renderText(">", 122, 37);
            renderText(">", 122, 57);
            renderItemIcon(TOOL_PICKAXE, 2, 130, 36); // Iron Pickaxe
            renderItemIcon(TOOL_PICKAXE, 3, 130, 56); // Gold Pickaxe
            break;
        }

        renderTextCentered(pageText, 6, width);
        if (pageNum > 0) {
            renderText("<", 1, 8);
            renderButtonIcon(localInputs.k_menuPrev.input & -localInputs.k_menuPrev.input, 8, 2);
        }
        if (pageNum < maxPageNum) {
            renderText(">", 153, 8);
            renderButtonIcon(localInputs.k_menuNext.input & -localInputs.k_menuNext.input, 136, 2);
        }
        renderTextCentered("Press   to exit", 109, width);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 62, 104);
    }
}
