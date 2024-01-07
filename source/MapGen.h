#pragma once
#include <3ds.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Globals.h"

extern float nextFloat();
extern double sample(double *values, int x, int y);
extern double *Noise(int w, int h, int featureSize);
extern void createAndValidateTopMap(int w, int h, int level, u8 *map, u8 *data);
extern void createTopMap(int w, int h, int level, u8 *map, u8 *data);
extern void createAndValidateUndergroundMap(int w, int h, int depthLevel, int level, u8 *map, u8 *data);
extern void createUndergroundMap(int w, int h, int depthLevel, int level, u8 *map, u8 *data);
extern void createAndValidateDungeonMap(int w, int h, int level, u8 *map, u8 *data);
extern void createDungeonMap(int w, int h, int level, u8 *map, u8 *data);
extern void createAndValidateSkyMap(int w, int h, int level, u8 *map, u8 *data);
extern void createSkyMap(int w, int h, int level, u8 *map, u8 *data);

extern int featureX;
extern int featureY;
extern void findFeatureLocation(int fw, int fh, int *accepted, int aLength, int maxTries, int w, int h, u8 *map, u8 *data);

extern void createVillage(int w, int h, int level, u8 *map, u8 *data);
extern void createDwarfHouse(int w, int h, int level, u8 *map, u8 *data);

extern bool hasNPC;
extern void createDungeonRoom(int w, int h, bool dragon, int level, u8 *map, u8 *data);