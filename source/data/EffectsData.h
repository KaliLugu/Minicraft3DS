#include <stdint.h>
#pragma once

#define vanillaEffectCount 6

typedef struct {
    int id;
    char *name;
    int iconX, iconY;
} EffectData;

#define EFFECTS_DURATION_INFINITE 429496729
extern EffectData *g_effectTable;
extern unsigned int g_effectCount;

extern void effectsDataInit();
extern void effectsTableBuild(uint16_t modCount);

extern char *effectGetName(int id);
extern int effectGetIconX(int id);
extern int effectGetIconY(int id);
extern int effectGetIdFromName(const char *name);
extern int effectGetLastId();
