#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "version.h"

static bool isValidVersionFormat(const char *version) {
    if (version == NULL) return false;
    int major, minor, patch;
    return sscanf(version, "%d.%d.%d", &major, &minor, &patch) == 3;
}

static int compareVersions(const char *a) {
    int major, minor, patch;
    if (sscanf(a, "%d.%d.%d", &major, &minor, &patch) != 3) return 0;
    if (major != VERSION_MAJOR) return major - VERSION_MAJOR;
    if (minor != VERSION_MINOR) return minor - VERSION_MINOR;
    return patch - VERSION_PATCH;
}

bool isSameVersion(const char *version) {
    if (!isValidVersionFormat(version)) return false;
    return compareVersions(version) == 0;  // Use numeric comparison, not strcmp
}

bool isOlderVersion(const char *version) {
    return compareVersions(version) < 0;
}

bool isNewerVersion(const char *version) {
    return compareVersions(version) > 0;
}