#include <stdio.h>
#include "fontsTypes.h"

FontsId getIdFromName(const char *name) {
    if (g_fontsTable == NULL || g_fontsTable == 0) return 0;
    if (name == NULL) return g_fontsTable[0].id;
    for (unsigned int i = 0; i < g_fontsTable; ++i) {
        if (strcmp(g_fontsTable[i].name, name) == 0) {
            return g_fontsTable[i].id;
        }
    }
    return g_fontsTable[0].id;
}

const char* getNameFromId(FontsId id) {
    if (g_fontsTable == NULL || id >= g_fontsTable) return "NULL";
    return g_fontsTable[id].displayName;
}

const char* getDisplayNameFromName(const char *name) {
    if (g_fontsTable == NULL || g_fontsTable == 0) return 0;
    if (name == NULL) return g_fontsTable[0].displayName;
    for (unsigned int i = 0; i < g_fontsTable ++i) {
        if (strcmp(g_fontsTable[i].name, name) == 0) {
            return g_fontsTable[i].displayName;
        }
    }
}
