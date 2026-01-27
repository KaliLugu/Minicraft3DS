#include "Quests.h"

#include "Globals.h"
#include "Render.h"

RecipeManager priestTrades;
RecipeManager farmerTrades;
RecipeManager dwarfTrades;

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

// todo use for cycle for trades
void initTrades() {
    priestTrades.size = 5;
    priestTrades.recipes = (Recipe *)malloc(sizeof(Recipe) * (priestTrades.size));
    priestTrades.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 33}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 38}), 2);
    priestTrades.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 34}), 1, 4, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 6}), 100, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 32}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 26}), 10);
    priestTrades.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_TOOL, 8}), 1, 2, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 10, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 17}), 5);
    priestTrades.recipes[3] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 11}), 5);
    priestTrades.recipes[4] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 0}), 5);

    farmerTrades.size = 7;
    farmerTrades.recipes = (Recipe *)malloc(sizeof(Recipe) * (farmerTrades.size));
    farmerTrades.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 10}), 5, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 3);
    farmerTrades.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 1}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 3);
    farmerTrades.recipes[2] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_FOOD, 2}), 2, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 4);
    farmerTrades.recipes[3] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 7}), 3, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 1);
    farmerTrades.recipes[4] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 9}), 4, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 2);
    farmerTrades.recipes[5] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 2, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 9}), 5);
    farmerTrades.recipes[6] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 39}), 1, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 7}), 5);

    dwarfTrades.size = 2;
    dwarfTrades.recipes = (Recipe *)malloc(sizeof(Recipe) * (dwarfTrades.size));
    dwarfTrades.recipes[0] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 4, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 1);
    dwarfTrades.recipes[1] = defineRecipe(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 15}), 2, 1, itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 18}), 1);
    // TODO: Trade Dragon Scales for something really nice
}

void freeTrades() {
    free(priestTrades.recipes);
    free(farmerTrades.recipes);
    free(dwarfTrades.recipes);
}

void initQuests(QuestlineManager *questManager) {
    if (questManager->questlines != NULL) {
        freeQuests(questManager);
    }

    questManager->size = 2;
    questManager->questlines = (Questline *)malloc(sizeof(Questline) * (questManager->size));
}

void resetQuests(QuestlineManager *questManager) {
    int i;
    for (i = 0; i < questManager->size; ++i) {
        questManager->questlines[i].currentQuest = 0;
        questManager->questlines[i].currentQuestDone = false;
    }
}

void freeQuests(QuestlineManager *questManager) {
    free(questManager->questlines);
    questManager->questlines = NULL;
}

void resetNPCMenuData(NPC_MenuData *data) {
    data->currentNPC = 0;
    data->currentNPCMenu = 0;
    data->currentNPCVal = 0;

    data->currentTalkSel = 0;
    data->currentTalkDone = false;
    data->currentTalkOptions = 0;

    data->currentTalkOption0 = "";
    data->currentTalkOption1 = "";
    data->currentTalkOption2 = "";
    data->currentTalk0 = "";
    data->currentTalk1 = "";
    data->currentTalk2 = "";
    data->currentTalk3 = "";
    data->currentTalk4 = "";
    data->currentTalk5 = "";
}

void openNPCMenu(PlayerData *pd, int npc) {
    pd->ingameMenu = MENU_NPC;

    NPC_MenuData *data = &(pd->npcMenuData);
    QuestlineManager *questManager = &(pd->questManager);

    data->currentNPC = npc;
    data->currentNPCVal = 0;
    data->currentNPCMenu = NPC_MENU_TALK;

    data->currentTalkSel = 0;
    data->currentTalkDone = false;
    data->currentTalkOptions = 1;
    data->currentTalkOption0 = "Bye";
    data->currentTalkOption1 = "";
    data->currentTalkOption2 = "";
    data->currentTalk0 = "";
    data->currentTalk1 = "";
    data->currentTalk2 = "";
    data->currentTalk3 = "";
    data->currentTalk4 = "";
    data->currentTalk5 = "";

    // TODO: Handle upon currentNPC as well as the fitting quest progress
    switch (data->currentNPC) {
    case NPC_GIRL:
        data->currentTalkOptions = 1;
        data->currentTalkOption0 = "...";

        data->currentTalk0 = "Hello?               ";
        data->currentTalk1 = "I have a feeling of  ";
        data->currentTalk2 = "forgetting something ";
        data->currentTalk3 = "very important.      ";
        data->currentTalk4 = "Hopefully I will     ";
        data->currentTalk5 = "remember it soon...  ";
        break;
    case NPC_PRIEST:
        data->currentTalkOptions = 3;
        data->currentTalkOption1 = "Trade";
        data->currentTalkOption2 = "Why are you so few?";

        data->currentTalk0 = "Welcome to my village";
        data->currentTalk1 = "I am our leader.     ";
        data->currentTalk2 = "If you have anything ";
        data->currentTalk3 = "usefull, I might be  ";
        data->currentTalk4 = "able to provide some-";
        data->currentTalk5 = "thing nice in trade. ";
        break;
    case NPC_FARMER:
        data->currentTalkOptions = 2;
        data->currentTalkOption0 = "some other time";
        data->currentTalkOption1 = "Trade";

        data->currentTalk0 = "Hello friend!        ";
        data->currentTalk1 = "Nice seeing somebody ";
        data->currentTalk2 = "else visit my little ";
        data->currentTalk3 = "farm. Interested in  ";
        data->currentTalk4 = "buying some fresh    ";
        data->currentTalk5 = "farm goods?          ";
        break;
    case NPC_LIBRARIAN:
        data->currentTalkOptions = 2;
        data->currentTalkOption0 = "Nothing";
        data->currentTalkOption1 = "What are you doing?";
        if (questManager->questlines[1].currentQuest == 1) {
            data->currentTalkOptions = 3;
            data->currentTalkOption2 = "Dwarvish language";
        }

        data->currentTalk0 = "Oh hello?            ";
        data->currentTalk1 = "You are quite brave  ";
        data->currentTalk2 = "or stupid walking    ";
        data->currentTalk3 = "around this dungeon. ";
        data->currentTalk4 = "                     ";
        data->currentTalk5 = "How can I help you?  ";
        break;
    case NPC_DWARF:
        if (questManager->questlines[1].currentQuest <= 1) {
            questManager->questlines[1].currentQuest = 1;

            data->currentTalkOptions = 1;
            data->currentTalkOption0 = "?";

            data->currentTalk0 = "Dwo neal bet reck da ";
            data->currentTalk1 = "dhum don lir lugn at ";
            data->currentTalk2 = "nur tor erno ur yo ta";
            data->currentTalk3 = "thra so tir kho ukk  ";
            data->currentTalk4 = "hel dro ic           ";
            data->currentTalk5 = "                     ";
            // TODO: set to 2 once translation book has been bought from librarian(can only be done once it is 1, so the dwarf has been found once)
        } else if (questManager->questlines[1].currentQuest == 2) {
            data->currentTalkOptions = 2;
            data->currentTalkOption0 = "Not really";
            data->currentTalkOption1 = "Trade";

            data->currentTalk0 = "How are ya?          ";
            data->currentTalk1 = "Pretty unusal meeting";
            data->currentTalk2 = "someone down here.   ";
            data->currentTalk3 = "                     ";
            data->currentTalk4 = "Have something       ";
            data->currentTalk5 = "valuable to trade?   ";
        }
        break;
    }
}

void tickTalkMenu(PlayerData *pd, NPC_MenuData *data) {
    if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked)
        pd->ingameMenu = MENU_NONE;

    if (pd->inputs.k_up.clicked) {
        ++data->currentTalkSel;
        if (data->currentTalkSel >= data->currentTalkOptions)
            data->currentTalkSel = 0;
    }
    if (pd->inputs.k_down.clicked) {
        --data->currentTalkSel;
        if (data->currentTalkSel < 0)
            data->currentTalkSel = data->currentTalkOptions - 1;
    }

    if (pd->inputs.k_accept.clicked) {
        data->currentTalkDone = true;
    }
}

void tickNPCMenu(PlayerData *pd) {
    NPC_MenuData *data = &(pd->npcMenuData);
    QuestlineManager *questManager = &(pd->questManager);

    // TODO: Handle upon currentNPC as well as the fitting quest progress
    if (data->currentNPCMenu == NPC_MENU_TALK)
        tickTalkMenu(pd, data);

    switch (data->currentNPC) {
    case NPC_GIRL:
        if (data->currentNPCMenu == NPC_MENU_TALK && data->currentTalkDone) {
            if (data->currentNPCVal == 0)
                pd->ingameMenu = MENU_NONE;
        }
        break;
    case NPC_PRIEST:
        if (data->currentNPCMenu == NPC_MENU_TALK && data->currentTalkDone) {
            if (data->currentNPCVal == 0) {
                if (data->currentTalkSel == 0) {
                    pd->ingameMenu = MENU_NONE;
                } else if (data->currentTalkSel == 1) {
                    openCraftingMenu(pd, &priestTrades, "Trading");
                } else if (data->currentTalkSel == 2) {
                    data->currentNPCVal = 1;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "...";

                    data->currentTalk0 = "For a long time this ";
                    data->currentTalk1 = "village has been     ";
                    data->currentTalk2 = "tyrannized by an Air ";
                    data->currentTalk3 = "Wizard. Only we have ";
                    data->currentTalk4 = "still not given up   ";
                    data->currentTalk5 = "our old homes.       ";
                }
            } else if (data->currentNPCVal == 1) {
                if (data->currentTalkSel == 0) {
                    data->currentNPCVal = 2;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "...";

                    data->currentTalk0 = "The wizard hides some";
                    data->currentTalk1 = "where in the cloudes.";
                    data->currentTalk2 = "They can only be     ";
                    data->currentTalk3 = "reached via stairs   ";
                    data->currentTalk4 = "protected by almost  ";
                    data->currentTalk5 = "undestroyable stones.";
                }
            } else if (data->currentNPCVal == 2) {
                if (data->currentTalkSel == 0) {
                    data->currentNPCVal = 3;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "...";

                    data->currentTalk0 = "I would guess you    ";
                    data->currentTalk1 = "need tools atleast as";
                    data->currentTalk2 = "strong as diamonds to";
                    data->currentTalk3 = "be able to destroy   ";
                    data->currentTalk4 = "them.                ";
                    data->currentTalk5 = "                     ";
                }
            } else if (data->currentNPCVal == 3) {
                if (data->currentTalkSel == 0) {
                    data->currentNPCVal = 4;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 2;
                    data->currentTalkOption0 = "Let me do it!";
                    data->currentTalkOption1 = "I am not sure";

                    data->currentTalk0 = "I'm willing to give  ";
                    data->currentTalk1 = "an artifact passed   ";
                    data->currentTalk2 = "down over generations";
                    data->currentTalk3 = "to anybody who can   ";
                    data->currentTalk4 = "defeat the wizard and";
                    data->currentTalk5 = "come back with proof.";
                }
            } else if (data->currentNPCVal == 4) {
                pd->ingameMenu = MENU_NONE;
            }
        }
        break;
    case NPC_FARMER:
        if (data->currentNPCMenu == NPC_MENU_TALK && data->currentTalkDone) {
            if (data->currentNPCVal == 0) {
                if (data->currentTalkSel == 0) {
                    pd->ingameMenu = MENU_NONE;
                } else if (data->currentTalkSel == 1) {
                    openCraftingMenu(pd, &farmerTrades, "Trading");
                }
            }
        }
        break;
    case NPC_LIBRARIAN:
        if (data->currentNPCMenu == NPC_MENU_TALK && data->currentTalkDone) {
            if (data->currentNPCVal == 0) {
                if (data->currentTalkSel == 0) {
                    pd->ingameMenu = MENU_NONE;
                } else if (data->currentTalkSel == 1) {
                    data->currentNPCVal = 2;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "Ok";

                    data->currentTalk0 = "Books in this dungeon";
                    data->currentTalk1 = "house secrets that   ";
                    data->currentTalk2 = "can't be found any-  ";
                    data->currentTalk3 = "where else in the    ";
                    data->currentTalk4 = "world. So I came to  ";
                    data->currentTalk5 = "study them.          ";
                } else if (data->currentTalkSel == 2) {
                    data->currentNPCVal = 1;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 2;
                    data->currentTalkOption0 = "I need to think about it";
                    data->currentTalkOption1 = "Here they are";

                    data->currentTalk0 = "You have met a dwarf ";
                    data->currentTalk1 = "but had a little     ";
                    data->currentTalk2 = "communication problem?";
                    data->currentTalk3 = "I have a translation ";
                    data->currentTalk4 = "book. For 10 Goldbars";
                    data->currentTalk5 = "I will give it to you.";
                }
            } else if (data->currentNPCVal == 1) {
                if (data->currentTalkSel == 0) {
                    pd->ingameMenu = MENU_NONE;
                } else if (data->currentTalkSel == 1) {
                    data->currentNPCVal = 2;

                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "";

                    if (countItemInv(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), 0, &(pd->inventory)) >= 10) {
                        // remove gold from player inventory
                        // TODO: Maybe I should make a generic substract items method sometime
                        Item *item = getItemFromInventory(itemGetLegacyId((ItemID){ITEM_CATEGORY_GENERIC, 16}), &(pd->inventory));
                        item->countLevel -= 10;
                        if (item->countLevel < 1)
                            removeItemFromInventory(item->slotNum, &(pd->inventory));

                        questManager->questlines[1].currentQuest = 2;

                        data->currentTalk0 = "Thank you these will ";
                        data->currentTalk1 = "be really helpfull.  ";
                        data->currentTalk2 = "Take the book with it";
                        data->currentTalk3 = "you should be able to";
                        data->currentTalk4 = "understand anything a";
                        data->currentTalk5 = "dwarf can say.       ";

                        data->currentTalkOption0 = "Thanks";
                    } else {
                        data->currentTalk0 = "You do not seem to   ";
                        data->currentTalk1 = "have enough Goldbars ";
                        data->currentTalk2 = "on you.              ";
                        data->currentTalk3 = "                     ";
                        data->currentTalk4 = "Meet me when you have";
                        data->currentTalk5 = "collected 10 Bars.   ";

                        data->currentTalkOption0 = "Ok";
                    }
                }
            } else if (data->currentNPCVal == 2) {
                if (data->currentTalkSel == 0) {
                    pd->ingameMenu = MENU_NONE;
                }
            }
        }
        break;
    case NPC_DWARF:
        if (questManager->questlines[1].currentQuest <= 1) {
            if (data->currentNPCMenu == NPC_MENU_TALK && data->currentTalkDone) {
                if (data->currentNPCVal == 0)
                    pd->ingameMenu = MENU_NONE;
            }
        } else if (questManager->questlines[1].currentQuest == 2) {
            if (data->currentNPCMenu == NPC_MENU_TALK && data->currentTalkDone) {
                if (data->currentTalkSel == 0) {
                    pd->ingameMenu = MENU_NONE;
                } else if (data->currentTalkSel == 1) {
                    openCraftingMenu(pd, &dwarfTrades, "Trading");
                }
            }
        }
        break;
    }
}

void renderTalkMenu(NPC_MenuData *data, char *name) {
    renderFrame(1, 1, 24, 14, 0x1010FFFF);
    renderTextColor(name, 12 + 1, 6 + 1, 0x000000FF);
    renderTextColor(name, 12, 6, 0xE2E26FFF);

    renderText(data->currentTalk0, 16, 16);
    renderText(data->currentTalk1, 16, 24);
    renderText(data->currentTalk2, 16, 32);
    renderText(data->currentTalk3, 16, 40);
    renderText(data->currentTalk4, 16, 48);
    renderText(data->currentTalk5, 16, 56);

    if (data->currentTalkOptions >= 3)
        renderText(data->currentTalkOption2, 32, 73);
    if (data->currentTalkOptions >= 2)
        renderText(data->currentTalkOption1, 32, 85);
    if (data->currentTalkOptions >= 1)
        renderText(data->currentTalkOption0, 32, 97);

    if (data->currentTalkOptions >= 3 && data->currentTalkSel == 2)
        renderText(">", 24, 73);
    if (data->currentTalkOptions >= 2 && data->currentTalkSel == 1)
        renderText(">", 24, 85);
    if (data->currentTalkOptions >= 1 && data->currentTalkSel == 0)
        renderText(">", 24, 97);
}

void renderNPCMenu(NPC_MenuData *data) {
    // TODO: Handle upon currentNPC as well as the fitting quest progress
    switch (data->currentNPC) {
    case NPC_GIRL:
        if (data->currentNPCMenu == NPC_MENU_TALK)
            renderTalkMenu(data, "Maria");
        break;
    case NPC_PRIEST:
        if (data->currentNPCMenu == NPC_MENU_TALK)
            renderTalkMenu(data, "Priest Brom");
        break;
    case NPC_FARMER:
        if (data->currentNPCMenu == NPC_MENU_TALK)
            renderTalkMenu(data, "Farmer Garrow");
        break;
    case NPC_LIBRARIAN:
        if (data->currentNPCMenu == NPC_MENU_TALK)
            renderTalkMenu(data, "Librarian Ajihad");
        break;
    case NPC_DWARF:
        if (data->currentNPCMenu == NPC_MENU_TALK)
            renderTalkMenu(data, "Dwarf Orik");
        break;
    }
}
