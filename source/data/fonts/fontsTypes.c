#include <stdlib.h>
#include "fontsTypes.h"

#define FONT_ENTRY(name, displayName) \
    {0, name, displayName}

static const fontsData _vanillaDefs[] = {
    FONT_ENTRY("DEFAULT", "Minicraft3ds default"),
};

static const unsigned int _vanillaCount = sizeof(_vanillaDefs) / sizeof(_vanillaDefs[0]);

fontsData *g_fontsTable = NULL;
unsigned int g_fontsCount;

void fontsTableBuild(uint8_t fontsCount) {
    free(g_fontsTable);
    g_fontsTable = NULL;

    g_fontsCount = _vanillaCount + fontsCount;
    if (g_fontsCount > MAX_FONTS_ID) {
        g_fontsCount = MAX_FONTS_ID; // to much fonts mods
    }

    g_fontsTable = calloc(g_fontsCount, sizeof(fontsData));
       if (!g_fontsTable) {
       g_fontsCount = 0;
       return;
   }

    for (unsigned int i = 0; i < _vanillaCount && i < g_fontsCount; i++) {
        g_fontsTable[i] = _vanillaDefs[i];
        g_fontsTable[i].id = (FontsId)i;
    }
}

void fontsTableFree(void) {
    free(g_fontsTable);
    g_fontsTable = NULL;
    g_fontsCount = 0;
}
