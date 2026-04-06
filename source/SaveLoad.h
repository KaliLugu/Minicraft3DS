#pragma once

#include "Entity.h"
#include "Player.h"
#include <stdio.h>
#include <string.h>

typedef struct _worldData WorldData;

#define SAVE_VERSION 2 // Increment this when making incompatible save changes.

#define SAVE_COMMENT "Minicraft3DSSave"
#define SAVE_COPYBUFFER_SIZE 4096

// Load error codes
#define LOAD_ERROR_NONE 0
#define LOAD_ERROR_VERSION_MISMATCH 1
#define LOAD_ERROR_LEGACY_SAVE 2
#define LOAD_ERROR_ZIP_FAILED 3
#define LOAD_ERROR_MISSING_WORLD 4
#define LOAD_ERROR_FILE_MISSING 5

extern bool renameWorld(char *oldFilename, char *newFilename);

extern bool saveWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);
extern bool loadWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);
extern void getWorldPlayerState(char *filename, sInt *score, bool *win);

extern bool isWorldNameValid(char *worldName);
extern int checkFileNameForErrors(char *filename);

extern int getLastLoadError();
