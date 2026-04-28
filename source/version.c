#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "version.h"
#include "cJSON.h"
#include "network/miniCurl.h"

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

// use version start with 0.x.x for error cases, so that it will be considered as older than any valid version and we can determinate error to show
char *getLatestRemoteVersion() {
    if (internetInit() != 0) {
        return "0.0.1";
    }
    char *json = miniCurlGet("https://api.github.com/repos/KaliLugu/Minicraft3DS/tags");
    if (!json) {
        exitInternet();
        return "0.0.2";
    }
    if (exitInternet() != 0) {
        free(json);
        return "0.0.3";
    }

    cJSON* request_json = NULL;
    cJSON* name = NULL;
    request_json = cJSON_Parse(json);
    if (!request_json) {
        free(json);
        return "0.0.4";
    }
    free(json);

    cJSON* first_tag = cJSON_GetArrayItem(request_json, 0);
    if (!first_tag) {
        cJSON_Delete(request_json);
        return "0.0.5";
    }

    name = cJSON_GetObjectItem(first_tag, "name");
    if (!name || !name->valuestring) {
        cJSON_Delete(request_json);
        return "0.0.6";
    }

    char *version = strdup(name->valuestring);
    cJSON_Delete(request_json);
    return version;
}
