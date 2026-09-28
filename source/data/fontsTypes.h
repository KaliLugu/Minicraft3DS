#pragma once
#include "../engine/dtypes.h"
#include <stdint.h>

#define MAX_FONTS_ID 255

typedef uint8_t FontsId;

typedef struct
{
    FontsId id; // id runtime
    const char* name;
    const char* displayName;
} fontsData;

extern unsigned int g_fontsCount;
extern fontsData *g_fontsTable;
extern void fontsTableBulid(uint8_t fontsCount);
