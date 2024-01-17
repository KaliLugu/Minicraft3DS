#pragma once
#include "engine/dtypes.h"

typedef struct _questline {
    int currentQuest;
    bool currentQuestDone;
} Questline;

typedef struct _questlineManager {
    int size;
    Questline *questlines;
} QuestlineManager;

typedef struct _npcMenuData {
    uByte currentNPC;

    int currentNPCMenu;
    int currentNPCVal;

    int currentTalkSel;
    bool currentTalkDone;
    int currentTalkOptions;
    char *currentTalkOption0;
    char *currentTalkOption1;
    char *currentTalkOption2;
    char *currentTalk0;
    char *currentTalk1;
    char *currentTalk2;
    char *currentTalk3;
    char *currentTalk4;
    char *currentTalk5;
} NPC_MenuData;

// TODO: Actually move the data here
