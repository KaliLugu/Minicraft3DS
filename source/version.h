#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_STRING "2.0.0"

extern char *getVersion();
extern bool isSameVersion(char *version);
extern bool isOlderVersion(char *version);
extern bool isNewerVersion(char *version);