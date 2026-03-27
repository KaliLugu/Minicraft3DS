#pragma once

#include <stdbool.h>

#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)

#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#define VERSION_STRING \
    TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH)

extern bool isSameVersion(const char *version);
extern bool isOlderVersion(const char *version);
extern bool isNewerVersion(const char *version);