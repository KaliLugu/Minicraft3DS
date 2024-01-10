#pragma once
#ifdef __3DS__

#include <3ds.h>
#include <citro2d.h>

#define MIN_TEXTURE_SIZE 64

typedef C3D_Tex *Image;
typedef struct {
    C2D_Image img;
    bool valid;
} Texture;
typedef unsigned int Color;

#endif