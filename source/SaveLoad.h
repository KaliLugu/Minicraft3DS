#pragma once

#include "Entity.h"
#include "Globals.h"
#include "Player.h"
#include <stdio.h>
#include <string.h>

#define SAVE_VERSION 2

#define SAVE_COMMENT "Minicraft3DSSave"
#define SAVE_COPYBUFFER_SIZE 4096

extern bool renameWorld(char *oldFilename, char *newFilename);

extern bool saveWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);
extern bool loadWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);
extern void getWorldPlayerState(char *filename, sInt *score, bool *win);

extern bool isWorldNameValid(char *worldName);
extern int checkFileNameForErrors(char *filename);
