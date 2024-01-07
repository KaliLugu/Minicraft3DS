#pragma once

#include "Entity.h"
#include "Globals.h"
#include "Player.h"
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define SAVE_VERSION 2

#define SAVE_COMMENT "Minicraft3DSSave"
#define SAVE_COPYBUFFER_SIZE 4096

extern bool saveFileCopy(char *target, char *source);

extern bool saveWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);
extern bool loadWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);

extern s8 checkFileNameForErrors(char *filename);
