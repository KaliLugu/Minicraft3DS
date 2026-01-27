#include "IngameMenu.h"

#include "Data.h"
#include "Globals.h"
#include "Ingame.h"
#include "Player.h"
#include "Render.h"
#include "SaveLoad.h"
#include "network/Synchronizer.h"

// to mose, see comment in main.c
int itemGetLegacyId(ItemID id)
{
    switch (id.category)
    {
        case ITEM_CATEGORY_TOOL:
            if (id.id >= toolItemCount) return 0;
            return toolItems[id.id].legacy_id;

        case ITEM_CATEGORY_FOOD:
            if (id.id >= foodItemCount) return 0;
            return foodItems[id.id].legacy_id;

        case ITEM_CATEGORY_GENERIC:
            if (id.id >= genericItemCount) return 0;
            return genericItems[id.id].legacy_id;

        default:
            return 0;
    }
}

char pOptions[][24] = {"Return to game", "Save Progress", "Exit to title"};

void ingameMenuTick(PlayerData *pd, int menu) {
    switch (menu) {
    case MENU_PAUSED:
        if (!pd->ingameMenuAreYouSure && !pd->ingameMenuAreYouSureSave) {
            if (pd->ingameMenuTimer > 0)
                --pd->ingameMenuTimer;
            if (pd->inputs.k_pause.clicked || pd->inputs.k_decline.clicked)
                pd->ingameMenu = MENU_NONE;
            if (pd->inputs.k_up.clicked) {
                --pd->ingameMenuSelection;
                if (pd->ingameMenuSelection < 0)
                    pd->ingameMenuSelection = 2;
            }
            if (pd->inputs.k_down.clicked) {
                ++pd->ingameMenuSelection;
                if (pd->ingameMenuSelection > 2)
                    pd->ingameMenuSelection = 0;
            }
            if (pd->inputs.k_accept.clicked) {
                switch (pd->ingameMenuSelection) {
                case 0:
                    pd->ingameMenu = MENU_NONE;
                    break;
                case 1:
                    if (!dungeonActive())
                        pd->ingameMenuAreYouSureSave = true;
                    break;
                case 2:
                    pd->ingameMenuAreYouSure = true;
                    break;
                }
            }
        } else if (pd->ingameMenuAreYouSureSave) {
            if (pd->inputs.k_accept.clicked) {
                pd->ingameMenuTimer = 60;

                if (playerLocalIndex == 0) {
                    saveWorld(currentFileName, &eManager, &worldData, players, playerCount);
                }
                pd->ingameMenuAreYouSureSave = false;
                pd->ingameMenuAreYouSure = false;
            } else if (pd->inputs.k_decline.clicked) {
                pd->ingameMenuAreYouSureSave = false;
                pd->ingameMenuAreYouSure = false;
            }
        } else if (pd->ingameMenuAreYouSure) {
            if (pd->inputs.k_accept.clicked) {
                pd->ingameMenuAreYouSure = false;
                pd->ingameMenuAreYouSureSave = false;

                exitGame();
            } else if (pd->inputs.k_decline.clicked) {
                pd->ingameMenuAreYouSure = false;
                pd->ingameMenuAreYouSureSave = false;
            }
        }
        break;
    case MENU_INVENTORY:
        if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked) {
            pd->ingameMenu = MENU_NONE;
            pd->activeItem = &noItem;
            pd->entity.p.isCarrying = false;
        }
        // inventory
        if (pd->ingameMenuSelection == 0) {
            if (pd->inputs.k_accept.clicked) { // Select item from inventory
                if (pd->inventory.lastSlot != 0) {
                    Item median = pd->inventory.items[pd->ingameMenuInvSel];         // create copy of item.
                    removeItemFromInventory(pd->ingameMenuInvSel, &(pd->inventory)); // remove original
                    pushItemToInventoryFront(median, &(pd->inventory));              // add copy to front
                    playerSetActiveItem(pd, &(pd->inventory.items[0]));              // active item = copy.
                }
                pd->ingameMenu = MENU_NONE;
            }
            if (pd->inputs.k_up.clicked) {
                --pd->ingameMenuInvSel;
                if (pd->ingameMenuInvSel < 0)
                    pd->ingameMenuInvSel = pd->inventory.lastSlot - 1;
            }
            if (pd->inputs.k_down.clicked) {
                ++pd->ingameMenuInvSel;
                if (pd->ingameMenuInvSel > pd->inventory.lastSlot - 1)
                    pd->ingameMenuInvSel = 0;
            }

            if (pd->inputs.k_right.clicked) {
                pd->ingameMenuSelection = 1;
            }
            // player stats
        } else if (pd->ingameMenuSelection == 1) {

            if (pd->inputs.k_left.clicked) {
                pd->ingameMenuSelection = 0;
            }
        }
        break;

    case MENU_CRAFTING:
        if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked)
            pd->ingameMenu = MENU_NONE;
        if (pd->inputs.k_accept.clicked) {
            if (craftItem(&(pd->currentRecipes), &(pd->currentRecipes.recipes[pd->ingameMenuInvSel]), &(pd->inventory))) {
                playSoundPositioned(snd_craft, pd->entity.level, pd->entity.x, pd->entity.y);
                // reset active item pointer, because it could posibly point to garbage now
                pd->activeItem = &noItem;
            }
        }
        if (pd->inputs.k_up.clicked) {
            --pd->ingameMenuInvSel;
            if (pd->ingameMenuInvSel < 0)
                pd->ingameMenuInvSel = pd->currentRecipes.size - 1;
        }
        if (pd->inputs.k_down.clicked) {
            ++pd->ingameMenuInvSel;
            if (pd->ingameMenuInvSel > pd->currentRecipes.size - 1)
                pd->ingameMenuInvSel = 0;
        }
        break;

    case MENU_WIN:
        if (pd->inputs.k_accept.clicked) {
            pd->ingameMenu = MENU_NONE;
            pd->entity.p.hasWon = false;
        }
        break;
    case MENU_LOSE:
        if (pd->inputs.k_accept.clicked) {
            pd->ingameMenu = MENU_NONE;
            pd->entity.p.isDead = false;
            pd->entity.p.health = 10;
            pd->entity.level = 1;
            playerSpawn(pd);
            // TODO: This canceled to main menu, but what should I do in multiplayer?
        }
        pd->entity.hurtTime = 10;
        break;

    case MENU_CONTAINER:
        if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked)
            pd->ingameMenu = MENU_NONE;

        if (pd->inputs.k_left.clicked) {
            pd->curChestEntityR = 0;
            int tmp = pd->ingameMenuInvSel;
            pd->ingameMenuInvSel = pd->ingameMenuInvSelOther;
            pd->ingameMenuInvSelOther = tmp;
        }
        if (pd->inputs.k_right.clicked) {
            pd->curChestEntityR = 1;
            int tmp = pd->ingameMenuInvSel;
            pd->ingameMenuInvSel = pd->ingameMenuInvSelOther;
            pd->ingameMenuInvSelOther = tmp;
        }

        Inventory *i1 = pd->curChestEntityR == 1 ? &(pd->inventory) : pd->curChestEntity->entityFurniture.inv;
        Inventory *i2 = pd->curChestEntityR == 0 ? &(pd->inventory) : pd->curChestEntity->entityFurniture.inv;
        int len = i1->lastSlot;
        if (pd->ingameMenuInvSel < 0)
            pd->ingameMenuInvSel = 0;
        if (pd->ingameMenuInvSel >= len)
            pd->ingameMenuInvSel = len - 1;
        if (pd->inputs.k_up.clicked)
            --pd->ingameMenuInvSel;
        if (pd->inputs.k_down.clicked)
            ++pd->ingameMenuInvSel;
        if (len == 0)
            pd->ingameMenuInvSel = 0;
        if (pd->ingameMenuInvSel < 0)
            pd->ingameMenuInvSel += len;
        if (pd->ingameMenuInvSel >= len)
            pd->ingameMenuInvSel -= len;

        if (pd->inputs.k_accept.clicked && len > 0) {
            Item *pullItem = &i1->items[pd->ingameMenuInvSel];
            Item pushItem = newItem(pullItem->id, pullItem->countLevel);
            pushItem.chestPtr = pullItem->chestPtr;
            pushItemToInventoryFront(pushItem, i2);
            if (i2 == &(pd->inventory)) {
                int newslot = pd->activeItem->slotNum + 1;
                pd->activeItem = &(pd->inventory.items[newslot]);
            } else if (pullItem == pd->activeItem) {
                pd->activeItem = &noItem;
            }
            removeItemFromCurrentInv(pullItem);
            if (pd->ingameMenuInvSel >= i1->lastSlot)
                pd->ingameMenuInvSel = i1->lastSlot - 1;
        }
        break;

    case MENU_DUNGEON:
        if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked)
            pd->ingameMenu = MENU_NONE;

        if (pd->inputs.k_accept.clicked) {
            if (pd->entity.level != 5) {
                Item *item = getItemFromInventory(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 33}), &(pd->inventory));
                if (item != NULL) {
                    --item->countLevel;
                    if (item->countLevel == 0) {
                        removeItemFromCurrentInv(item);
                    }

                    enterDungeon(pd);
                } else if (TESTGODMODE) {
                    enterDungeon(pd);
                }
            } else {
                leaveDungeon(pd);
            }

            pd->ingameMenu = MENU_NONE;
        }
        break;

    case MENU_NPC:
        tickNPCMenu(pd);
        break;

    case MENU_CHARACTER_CUSTOMIZE:
        if (pd->inputs.k_up.clicked) {
            --pd->ingameMenuSelection;
            if (pd->ingameMenuSelection < 0)
                pd->ingameMenuSelection = 7;
        }
        if (pd->inputs.k_down.clicked) {
            ++pd->ingameMenuSelection;
            if (pd->ingameMenuSelection > 7)
                pd->ingameMenuSelection = 0;
        }

        uByte wrap = 0;
        wrap = wrap - 1;

        pd->entity.p.health = 10;
        pd->entity.hurtTime = 10;

        // head
        if (pd->ingameMenuSelection == 0) {
            if (pd->inputs.k_left.clicked) {
                --pd->sprite.head;
                if (pd->sprite.head == wrap)
                    pd->sprite.head = PLAYER_SPRITE_HEAD_COUNT - 1;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->sprite.head;
                if (pd->sprite.head > PLAYER_SPRITE_HEAD_COUNT - 1)
                    pd->sprite.head = 0;
            }
            // eyes
        } else if (pd->ingameMenuSelection == 1) {
            if (pd->inputs.k_left.clicked) {
                --pd->sprite.eyes;
                if (pd->sprite.eyes == wrap)
                    pd->sprite.eyes = PLAYER_SPRITE_EYES_COUNT - 1;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->sprite.eyes;
                if (pd->sprite.eyes > PLAYER_SPRITE_EYES_COUNT - 1)
                    pd->sprite.eyes = 0;
            }
            // body
        } else if (pd->ingameMenuSelection == 2) {
            if (pd->inputs.k_left.clicked) {
                --pd->sprite.body;
                if (pd->sprite.body == wrap)
                    pd->sprite.body = PLAYER_SPRITE_BODY_COUNT - 1;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->sprite.body;
                if (pd->sprite.body > PLAYER_SPRITE_BODY_COUNT - 1)
                    pd->sprite.body = 0;
            }
            // arms
        } else if (pd->ingameMenuSelection == 3) {
            if (pd->inputs.k_left.clicked) {
                --pd->sprite.arms;
                if (pd->sprite.arms == wrap)
                    pd->sprite.arms = PLAYER_SPRITE_ARMS_COUNT - 1;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->sprite.arms;
                if (pd->sprite.arms > PLAYER_SPRITE_ARMS_COUNT - 1)
                    pd->sprite.arms = 0;
            }
            // legs
        } else if (pd->ingameMenuSelection == 4) {
            if (pd->inputs.k_left.clicked) {
                --pd->sprite.legs;
                if (pd->sprite.legs == wrap)
                    pd->sprite.legs = PLAYER_SPRITE_LEGS_COUNT - 1;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->sprite.legs;
                if (pd->sprite.legs > PLAYER_SPRITE_LEGS_COUNT - 1)
                    pd->sprite.legs = 0;
            }
            // accs
        } else if (pd->ingameMenuSelection == 5) {
            if (pd->inputs.k_left.clicked) {
                --pd->sprite.accs;
                if (pd->sprite.accs == wrap)
                    pd->sprite.accs = PLAYER_SPRITE_ACCS_COUNT - 1;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->sprite.accs;
                if (pd->sprite.accs > PLAYER_SPRITE_ACCS_COUNT - 1)
                    pd->sprite.accs = 0;
            }
            // rotation
        } else if (pd->ingameMenuSelection == 6) {
            if (pd->inputs.k_left.clicked) {
                --pd->entity.p.dir;
                if (pd->entity.p.dir == wrap)
                    pd->entity.p.dir = 3;
            }
            if (pd->inputs.k_right.clicked) {
                ++pd->entity.p.dir;
                if (pd->entity.p.dir > 3)
                    pd->entity.p.dir = 0;
            }
            // done
        } else if (pd->ingameMenuSelection == 7) {
            if (pd->inputs.k_accept.clicked) {
                // TODO: are you sure dialog?
                pd->ingameMenu = 0;
                pd->ingameMenuSelection = 0;
                pd->sprite.choosen = true;
            }
        }
        break;
    }
}

uByte opacity = 255;
bool rev = true;
char scoreText[15];

void ingameMenuRender(PlayerData *pd, int menu, int screen, int width, int height) {
    int i;
    int oox, ooy;

    switch (menu) {
    case MENU_PAUSED:
        renderFrame(1, 1, 24, 14, 0xAF1010FF);
        renderTextCentered("Paused", 16, width);
        for (i = 3; i >= 0; --i) {
            char *msg = pOptions[i];
            MColor color = 0x7F7F7FFF;
            if (i == pd->ingameMenuSelection)
                color = 0xFFFFFFFF;
            if ((i == 1 && dungeonActive())) {
                color = 0x7F7FFFFF;
                if (i == pd->ingameMenuSelection)
                    color = 0xFFAFAFFF;
            }
            renderTextColor(msg, (width / 2 - (strlen(msg) * 8)) / 2, (i * 12) + 44, color);
        }

        if (pd->ingameMenuTimer > 0)
            renderTextColor("Game Saved!", (width / 2 - (11 * 8)) / 2, 32, 0x20FF20FF);

        if (pd->ingameMenuAreYouSure || pd->ingameMenuAreYouSureSave) {
            if (pd->ingameMenuAreYouSure)
                renderFrame(5, 5, 20, 12, 0x8F1010FF);
            else
                renderFrame(5, 5, 20, 12, 0x108F10FF);

            renderTextCentered("Are you sure?", 48, width);
            renderTextCentered("   Yes", 64, width);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 83, 64 - 4);
            renderTextCentered("   No", 80, width);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 83, 80 - 4);
        }
        break;
    case MENU_WIN:
        renderFrame(5, 3, 21, 12, 0x1010FFFF);
        if (!rev) {
            opacity += 5;
            if (opacity == 255)
                rev = true;
        } else {
            opacity -= 5;
            if (opacity == 100)
                rev = false;
        }
        sprintf(scoreText, "Score: %d", pd->score);
        renderTextColor("You Win!", (width / 2 - 8 * 8) / 2, 38, 0xAFAF0000 + opacity);
        renderTextCentered(scoreText, 50, width);
        renderTextCentered("Press   to continue", 75, width);
        renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 74, 70);
        break;
    case MENU_LOSE:
        renderFrame(5, 3, 21, 12, 0x1010FFFF);
        if (!rev) {
            opacity += 5;
            if (opacity == 255)
                rev = true;
        } else {
            opacity -= 5;
            if (opacity == 100)
                rev = false;
        }
        sprintf(scoreText, "Score: %d", pd->score);
        renderTextColor("You DIED!", (width / 2 - 9 * 8) / 2, 38, 0xAF000000 + opacity);
        renderTextCentered(scoreText, 50, width);
        renderTextCentered("Press   to continue", 75, width);
        renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 74, 70);
        break;
    case MENU_INVENTORY:
        if (pd->ingameMenuSelection == 1) {
            offsetX = 176;
            offsetY = 0;
        } else {
            offsetX = 0;
            offsetY = 0;
        }

        // inventory
        renderFrame(1, 1, 23, 14, 0x1010FFFF);
        renderTextColor("Inventory", 12 + 1, 6 + 1, 0x000000FF);
        renderTextColor("Inventory", 12, 6, 0xE2E26FFF);
        renderItemList(&(pd->inventory), 1, 1, 23, 14, pd->ingameMenuInvSel);

        // player stats
        renderFrame(24, 1, 46, 14, 0x1010FFFF);
        renderTextColor("Character", 184 + 12 + 1, 6 + 1, 0x000000FF);
        renderTextColor("Character", 184 + 12, 6, 0xE2E26FFF);

        // player sprite (TODO: Mostly Duplicated in MENU_CHARACTER_CUSTOMIZE, move to function?)
        renderFrame(25, 2, 34, 10, 0x909090FF);
        oox = offsetX;
        ooy = offsetY;
        // move player sprite to 0/0
        offsetX = offsetX + pd->entity.x - 8;
        offsetY = offsetY + pd->entity.y - 8;
        // move to where I want it
        offsetX -= 212;
        offsetY -= 24;
        renderPlayer(pd, 3);
        offsetX = oox;
        offsetY = ooy;

        // TODO: equipment

        // effects
        renderTextColor("Effects", 180 + 104 + 1, 17 + 1, 0x000000FF);
        renderTextColor("Effects", 180 + 104, 17, 0xFFFFFFFF);
        int epos = 0;
        for (i = 0; i < EFFECTS_MAX; i++) {
            if (playerEffectActive(pd, i)) {
                // can only display a limited amount of effects at once
                if (epos == 3) {
                    renderTextColor("...", 181 + 104, 17 + 12 + epos * 24, 0xFFFFFFFF);
                    break;
                }

                // draw information
                renderFrame(35, 3 + epos * 3, 45, 3 + epos * 3 + 3, 0x1010FFFF);

                sInt etime = playerEffectGetTime(pd, i);
                if (etime != EFFECTS_DURATION_INFINITE) {
                    char etimest[20];
                    sprintf(etimest, "       %02lu:%02lu", (etime / 60 / 60) % 60, (etime / 60) % 60);

                    renderTextColor(etimest, 181 + 104, 17 + 12 + epos * 24, 0xFFFFFFFF);
                }
                renderTile8((362 + 208) >> 1, (34 + 23 + epos * 48) >> 1, effectGetIconX(i), effectGetIconY(i), 0);
                renderTextColor(effectGetName(i), 181 + 104, 17 + 20 + epos * 24, 0xFFFFFFFF);

                epos++;
            }
        }

        offsetX = 0;
        offsetY = 0;
        break;
    case MENU_CRAFTING:
        renderFrame(15, 1, 24, 4, 0x1010FFFF);
        renderTextColor("Have", 124 + 1, 6 + 1, 0x000000FF);
        renderTextColor("Have", 124, 6, 0xE2E26FFF);
        renderFrame(15, 5, 24, 14, 0x1010FFFF);
        renderTextColor("Cost", 124 + 1, 38 + 1, 0x000000FF);
        renderTextColor("Cost", 124, 38, 0xE2E26FFF);
        renderFrame(1, 1, 14, 14, 0x1010FFFF);
        renderTextColor(pd->currentCraftTitle, 12 + 1, 6 + 1, 0x000000FF);
        renderTextColor(pd->currentCraftTitle, 12, 6, 0xE2E26FFF);
        renderRecipes(&(pd->currentRecipes), 1, 1, 14, 14, pd->ingameMenuInvSel);

        Recipe *rec = &(pd->currentRecipes.recipes[pd->ingameMenuInvSel]);
        renderItemIcon(rec->itemResult, rec->itemAmountLevel, 128, 16);
        char craftText[12];
        sprintf(craftText, "%d", countItemInv(rec->itemResult, rec->itemAmountLevel, &(pd->inventory)));
        renderText(craftText, 137, 17);

        if (rec->numOfCosts > 0) {
            int i;
            for (i = 0; i < rec->numOfCosts; i++) {
                int amnt = countItemInv(rec->costs[i].costItem, 0, &(pd->inventory));
                int ttlCst = rec->costs[i].costAmount;
                int col = 0xFFFFFFFF;
                if (amnt < ttlCst)
                    col = 0x7F7F7FFF;
                renderItemIcon(rec->costs[i].costItem, 1, 128, 48 + (i * 8));
                sprintf(craftText, "%d/%d", amnt, ttlCst);
                renderTextColor(craftText, 137, 48 + (i * 8), col);
            }
        }
        break;

    case MENU_CONTAINER:
        if (pd->curChestEntityR == 1) {
            offsetX = 128;
            offsetY = 0;
        } else {
            offsetX = 0;
            offsetY = 0;
        }

        renderFrame(1, 1, 20, 14, 0x1010FFFF);
        renderTextColor("Chest", 12 + 1, 6 + 1, 0x000000FF);
        renderTextColor("Chest", 12, 6, 0xE2E26FFF);
        renderItemList(pd->curChestEntity->entityFurniture.inv, 1, 1, 20, 14,
                       pd->curChestEntityR == 0 ? pd->ingameMenuInvSel : -pd->ingameMenuInvSelOther - 1);
        renderFrame(21, 1, 40, 14, 0x1010FFFF);
        renderTextColor("Inventory", 172 + 1, 6 + 1, 0x000000FF);
        renderTextColor("Inventory", 172, 6, 0xE2E26FFF);
        renderItemList(&(pd->inventory), 21, 1, 40, 14,
                       pd->curChestEntityR == 1 ? pd->ingameMenuInvSel : -pd->ingameMenuInvSelOther - 1);
        offsetX = 0;
        offsetY = 0;
        break;

    case MENU_DUNGEON:
        renderFrame(1, 1, 24, 14, 0x1010FFFF);
        if (pd->entity.level != 5) {
            renderTextColor("Dungeon Entrance", 12 + 1, 6 + 1, 0x000000FF);
            renderTextColor("Dungeon Entrance", 12, 6, 0xE2E26FFF);

            renderText("Warning: ", 16, 16);
            renderText("You need a Dungeonkey", 16, 28);
            renderText("to enter and cannot  ", 16, 36);
            renderText("save while in it!    ", 16, 44);
            renderText("After leaving you    ", 16, 56);
            renderText("need a new Dungeonkey", 16, 64);
            renderText("to enter another one!", 16, 72);

            renderText("   Enter", 74, 85);
        } else {
            renderTextColor("Dungeon Exit", 12 + 1, 6 + 1, 0x000000FF);
            renderTextColor("Dungeon Exit", 12, 6, 0xE2E26FFF);

            renderText("Warning: ", 16, 16);
            renderText("The Dungeon and every", 16, 28);
            renderText("thing in it will dis-", 16, 36);
            renderText("appear when you leave", 16, 44);
            renderText("You will need a new  ", 16, 56);
            renderText("Dungeonkey to enter  ", 16, 64);
            renderText("another one again!   ", 16, 72);

            renderText("   Leave", 74, 85);
        }

        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 75, 85 - 4);
        renderText("   Stay", 74, 97);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 75, 97 - 4);
        break;

    case MENU_NPC:
        renderNPCMenu(&(pd->npcMenuData));
        break;

    case MENU_CHARACTER_CUSTOMIZE:
        renderFrame(1, 1, 24, 14, 0x1010FFFF);
        renderTextColor("Character", 12 + 1, 6 + 1, 0x000000FF);
        renderTextColor("Character", 12, 6, 0xE2E26FFF);

        renderText("Head ", 16, 26);
        renderText("Eyes ", 16, 34);
        renderText("Body ", 16, 42);
        renderText("Arms ", 16, 50);
        renderText("Legs ", 16, 58);
        renderText("Accs ", 16, 66);

        renderText("Rot. ", 16, 79);

        // for the dynamic part
        char display[30];

        sprintf(display, pd->ingameMenuSelection == 0 ? "<%02i/%02i>" : " %02i/%02i ", pd->sprite.head + 1, PLAYER_SPRITE_HEAD_COUNT);
        renderText(display, 48, 26);
        sprintf(display, pd->ingameMenuSelection == 1 ? "<%02i/%02i>" : " %02i/%02i ", pd->sprite.eyes + 1, PLAYER_SPRITE_EYES_COUNT);
        renderText(display, 48, 34);
        sprintf(display, pd->ingameMenuSelection == 2 ? "<%02i/%02i>" : " %02i/%02i ", pd->sprite.body + 1, PLAYER_SPRITE_BODY_COUNT);
        renderText(display, 48, 42);
        sprintf(display, pd->ingameMenuSelection == 3 ? "<%02i/%02i>" : " %02i/%02i ", pd->sprite.arms + 1, PLAYER_SPRITE_ARMS_COUNT);
        renderText(display, 48, 50);
        sprintf(display, pd->ingameMenuSelection == 4 ? "<%02i/%02i>" : " %02i/%02i ", pd->sprite.legs + 1, PLAYER_SPRITE_LEGS_COUNT);
        renderText(display, 48, 58);
        sprintf(display, pd->ingameMenuSelection == 5 ? "<%02i/%02i>" : " %02i/%02i ", pd->sprite.accs + 1, PLAYER_SPRITE_ACCS_COUNT);
        renderText(display, 48, 66);
        sprintf(display, pd->ingameMenuSelection == 6 ? "<%02i/%02i>" : " %02i/%02i ", pd->entity.p.dir + 1, 4);
        renderText(display, 48, 79);
        sprintf(display, pd->ingameMenuSelection == 7 ? "<Done >" : " Done  ");
        renderText(display, 48, 87);

        // player sprite
        renderFrame(13, 3, 22, 12, 0x909090FF);
        oox = offsetX;
        ooy = offsetY;
        // move player sprite to 0/0
        offsetX = pd->entity.x - 8;
        offsetY = pd->entity.y - 8;
        // move to where I want it
        offsetX -= 108;
        offsetY -= 28;
        renderPlayer(pd, 4);

        offsetX = oox;
        offsetY = ooy;
        break;
    }
}

// touch menu
void tickTouchMap(PlayerData *pd) {
    if (pd->mapShouldRender) {
        if (pd->inputs.k_touchX > 0 || pd->inputs.k_touchY > 0) {
            // Plus/Minus zoom button
            if (pd->inputs.k_touchY > 204 && pd->inputs.k_touchY < 232) {
                if (pd->inputs.k_touchX > 284 && pd->inputs.k_touchX < 312) {
                    if (pd->mapZoomLevel > 4)
                        return;
                    if (!pd->touchIsChangingSize && !pd->touchIsDraggingMap) {
                        pd->mapZoomLevel += 2;
                        pd->mapScrollX -= (50 * (pd->mapZoomLevel / 2));
                        pd->mapScrollY -= (40 * (pd->mapZoomLevel / 2));
                        pd->touchIsChangingSize = true;
                        sprintf(pd->mapText, "x%d", pd->mapZoomLevel);
                    }
                    if (pd->mapScrollX < 320 - (128 * pd->mapZoomLevel))
                        pd->mapScrollX = 320 - (128 * pd->mapZoomLevel);
                    else if (pd->mapScrollX > 0)
                        pd->mapScrollX = 0;
                    if (pd->mapScrollY < 240 - (128 * pd->mapZoomLevel))
                        pd->mapScrollY = 240 - (128 * pd->mapZoomLevel);
                    else if (pd->mapScrollY > 0)
                        pd->mapScrollY = 0;
                    return;
                } else if (pd->inputs.k_touchX > 256 && pd->inputs.k_touchX < 284) {
                    if (pd->mapZoomLevel < 4)
                        return;
                    if (!pd->touchIsChangingSize && !pd->touchIsDraggingMap) {
                        pd->mapScrollX += (50 * (pd->mapZoomLevel / 2));
                        pd->mapScrollY += (40 * (pd->mapZoomLevel / 2));
                        pd->mapZoomLevel -= 2;
                        pd->touchIsChangingSize = true;
                        sprintf(pd->mapText, "x%d", pd->mapZoomLevel);
                    }
                    if (pd->mapScrollX < 320 - (128 * pd->mapZoomLevel))
                        pd->mapScrollX = 320 - (128 * pd->mapZoomLevel);
                    else if (pd->mapScrollX > 0)
                        pd->mapScrollX = 0;
                    if (pd->mapScrollY < 240 - (128 * pd->mapZoomLevel))
                        pd->mapScrollY = 240 - (128 * pd->mapZoomLevel);
                    else if (pd->mapScrollY > 0)
                        pd->mapScrollY = 0;
                    return;
                }
            } else if (pd->inputs.k_touchY > 8 && pd->inputs.k_touchY < 40 && pd->inputs.k_touchX > 284 && pd->inputs.k_touchX < 312) {
                // Exit Button
                if (!pd->touchIsChangingSize && !pd->touchIsDraggingMap) {
                    pd->mapShouldRender = false;
                    return;
                }
            }

            if (!pd->touchIsDraggingMap) {
                pd->touchLastX = pd->inputs.k_touchX;
                pd->touchLastY = pd->inputs.k_touchY;
            }
            if (pd->mapZoomLevel > 2) {
                int dx = pd->touchLastX - pd->inputs.k_touchX;
                if (dx > 1 || dx < -1) {
                    pd->mapScrollX -= dx;
                    if (pd->mapScrollX < 320 - (128 * pd->mapZoomLevel))
                        pd->mapScrollX = 320 - (128 * pd->mapZoomLevel);
                    else if (pd->mapScrollX > 0)
                        pd->mapScrollX = 0;
                }
                pd->touchLastX = pd->inputs.k_touchX;
            }

            int dy = pd->touchLastY - pd->inputs.k_touchY;
            if (dy > 1 || dy < -1) {
                pd->mapScrollY -= dy;
                if (pd->mapScrollY < 240 - (128 * pd->mapZoomLevel))
                    pd->mapScrollY = 240 - (128 * pd->mapZoomLevel);
                else if (pd->mapScrollY > 0)
                    pd->mapScrollY = 0;
            }
            pd->touchLastY = pd->inputs.k_touchY;
            pd->touchIsDraggingMap = true;
        } else {
            pd->touchIsDraggingMap = false;
            pd->touchIsChangingSize = false;
        }
    } else {
        // touch minimap to bring up zoomed map.
        if (pd->inputs.k_touchY > 100 && pd->inputs.k_touchY < 228 && pd->inputs.k_touchX > 10 && pd->inputs.k_touchX < 142) {
            pd->mapShouldRender = true;
        }
    }
}

void tickTouchQuickSelect(PlayerData *pd) {
    if (pd->ingameMenu == MENU_NONE && !pd->mapShouldRender) {
        int i = 0;
        Inventory *inv = &(pd->inventory);

        for (i = 0; i < 8; ++i) {
            if ((inv->lastSlot) > i) {
                int xip = i % 4;
                int yip = i / 4;

                if (pd->inputs.k_touchY > 72 * 2 + yip * 21 * 2 && pd->inputs.k_touchY < 72 * 2 + yip * 21 * 2 + 21 * 2 && pd->inputs.k_touchX > 76 * 2 + xip * 21 * 2 && pd->inputs.k_touchX < 76 * 2 + xip * 21 * 2 + 21 * 2) {
                    playerSetActiveItem(pd, &inv->items[i]);
                }
            }
        }
    }
}

void ingameMenuTickTouch(PlayerData *pd) {
    tickTouchMap(pd);
    tickTouchQuickSelect(pd);
}
