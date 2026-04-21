#include "EffectsData.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


// FUTUR TODO MODDING : in futur make effect système work with mdos

#define EFFECT_ENTRY(name, iconX, iconY) \
    {0, name, iconX, iconY}

static EffectData _effectDefs[] = {
    // Vanilla effects
    EFFECT_ENTRY("undying", 0, 44),
    EFFECT_ENTRY("regeneration", 1, 44),
    EFFECT_ENTRY("speed", 2, 44),
    EFFECT_ENTRY("strength", 3, 44),
    EFFECT_ENTRY("shielding", 4, 44),
    EFFECT_ENTRY("nightVision", 5, 44),
};

static const unsigned int _vanillaEffectCount = sizeof(_effectDefs) / sizeof(_effectDefs[0]);

EffectData *g_effectTable = NULL;
unsigned int g_effectCount;

void effectsTableBuild(uint16_t modCount) {
    g_effectCount = _vanillaEffectCount + modCount;
    g_effectTable = malloc(g_effectCount * sizeof(EffectData));
    if (!g_effectTable) {
        g_effectCount = 0;
        return;
    }
    for (unsigned int i = 0; i < _vanillaEffectCount; i++) {
        g_effectTable[i] = _effectDefs[i];
        g_effectTable[i].id = (int)i;
    }
}

char *effectGetName(int id) {
    return g_effectTable[id].name;
}

int effectGetIconX(int id) {
    return g_effectTable[id].iconX;
}

int effectGetIconY(int id) {
    return g_effectTable[id].iconY;
}

int effectGetIdFromName(const char *name) {
    for (unsigned int i = 0; i < g_effectCount; i++) {
        if (strcmp(g_effectTable[i].name, name) == 0) {
            return g_effectTable[i].id;
        }
    }
    return -1; // Not found
}

int effectGetLastId() {
    return (int)_vanillaEffectCount - 1;
}
