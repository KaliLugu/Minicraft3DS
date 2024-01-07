#pragma once

#include "Crafting.h"
#include "Player.h"
#include "QuestsData.h"
#include <stdbool.h>
#include <stdlib.h>

#define NPC_MENU_TALK 0

extern RecipeManager priestTrades;
extern RecipeManager farmerTrades;
extern RecipeManager dwarfTrades;

extern void initTrades();
extern void freeTrades();

extern void initQuests(QuestlineManager *questManager);
extern void resetQuests(QuestlineManager *questManager);
extern void freeQuests(QuestlineManager *questManager);

extern void resetNPCMenuData(NPC_MenuData *data);
extern void openNPCMenu(PlayerData *pd, int npc);
extern void tickNPCMenu(PlayerData *pd);
extern void renderNPCMenu(NPC_MenuData *data);
