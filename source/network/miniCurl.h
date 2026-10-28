#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>
#include "../engine/engine.h"

extern int internetInit();
extern int exitInternet();
extern char *miniCurlGet(const char *url);
