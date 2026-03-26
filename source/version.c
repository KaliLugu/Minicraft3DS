#include <stdbool.h>

#include "version.h"
#include "Globals.h"

char *getVersion() {
    return VERSION_STRING;
}

bool isSameVersion(char *version) {
    return strcmp(version, VERSION_STRING) == 0;
}

bool isOlderVersion(char *version) {
    return strcmp(version, VERSION_STRING) < 0;
}

bool isNewerVersion(char *version) {
    return strcmp(version, VERSION_STRING) > 0;
}