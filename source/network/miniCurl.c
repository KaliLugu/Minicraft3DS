#include "miniCurl.h"

typedef struct {
    char *string;
    size_t size;
} Response;

size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata);

char *miniCurlGet(const char *url) {
    // need to verify access to romfs
    // check certificate validation
    // process url
    // return response string
}