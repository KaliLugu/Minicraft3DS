#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Globals.h"

extern float nextFloat();
extern double sample(double *values, int x, int y);
extern double *Noise(int w, int h, int featureSize);
extern void createAndValidateTopMap(int w, int h, uByte level, uByte *map, uByte *data);
extern void createTopMap(int w, int h, uByte level, uByte *map, uByte *data);
extern void createAndValidateUndergroundMap(int w, int h, int depthLevel, uByte level, uByte *map, uByte *data);
extern void createUndergroundMap(int w, int h, int depthLevel, uByte level, uByte *map, uByte *data);
extern void createAndValidateDungeonMap(int w, int h, uByte level, uByte *map, uByte *data);
extern void createDungeonMap(int w, int h, uByte level, uByte *map, uByte *data);
extern void createAndValidateSkyMap(int w, int h, uByte level, uByte *map, uByte *data);
extern void createSkyMap(int w, int h, uByte level, uByte *map, uByte *data);

extern int featureX;
extern int featureY;
extern void findFeatureLocation(int fw, int fh, int *accepted, int aLength, int maxTries, int w, int h, uByte *map, uByte *data);

extern void createVillage(int w, int h, uByte level, uByte *map, uByte *data);
extern void createDwarfHouse(int w, int h, uByte level, uByte *map, uByte *data);

extern bool hasNPC;
extern void createDungeonRoom(int w, int h, bool dragon, uByte level, uByte *map, uByte *data);