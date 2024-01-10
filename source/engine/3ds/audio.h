#pragma once

#include "../engine.h"
#ifdef __3DS__

#include <3ds.h>

typedef struct {
    s16 *samples;
    u32 count;
} _3ds_sound;
typedef _3ds_sound *Sound;

typedef struct {
    u32 size;
    u32 pos;
    char *name;
} _3ds_music;
typedef _3ds_music *Music;
#endif
