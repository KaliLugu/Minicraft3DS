#pragma once

#include "Globals.h"
#include "minizip/unzip.h"
#include <ctype.h>
#include <string.h>

extern int loadTexturePack(char *filename);
extern int getTexturePackComment(char *filename, char *cmmtBuf);
