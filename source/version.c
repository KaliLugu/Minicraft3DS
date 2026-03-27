#include <stdbool.h>
#include <string.h>

#include "version.h"

bool buffSizeValidation(const char *version) {
    if (strlen(version) == strlen(VERSION_STRING)) {
        return true;
    }
    return false;
}

bool isSameVersion(const char *version) {
    if (!buffSizeValidation(version)) {
        return false;
    } else {
        return strcmp(version, VERSION_STRING) == 0;
    }

}

bool isOlderVersion(const char *version) {
    return strcmp(version, VERSION_STRING) < 0;
}

bool isNewerVersion(const char *version) {
    return strcmp(version, VERSION_STRING) > 0;
}