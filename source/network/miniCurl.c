#include "miniCurl.h"

typedef struct {
    char *string;
    size_t size;
} Response;

int internetInit() {
    if (R_FAILED(ret)) {
        fprintf(stderr, "httpcInit failed: 0x%08lX\n", ret);
        return -1;
    }
    socInit((u32*)memalign(0x1000, 0x100000), 0x100000); // socket memory
    return 0;
}

size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata);

char *miniCurlGet(const char *url) {
    // need to verify access to romfs
    // check certificate validation
    // process url
    // return response string
}