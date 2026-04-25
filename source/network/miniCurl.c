#include "miniCurl.h"

static u32 *_socBuffer = NULL;

typedef struct {
    char *string;
    size_t size;
} Response;


size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata);

int internetInit() {
    Result ret = httpcInit(0);
    if (R_FAILED(ret)) {
        fprintf(stderr, "httpcInit failed: 0x%08lX\n", ret);
        return -1;
    }
    socInit((u32*)memalign(0x1000, 0x100000), 0x100000); // socket memory
    return 0;
}

int exitInternet() {
    if (_socBuffer) {
        free(_socBuffer);
        _socBuffer = NULL;
    }
    return 0;
}



char *miniCurlGet(const char *url) {
    Response response;
    response.string = malloc(1);
    response.size = 0;

    CURL *curl;
    CURLcode result;

    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
        return NULL;
    }
    if (curl == NULL) {
        fprintf(stderr, "curl_easy_init() failed\n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "romfs:/cacert.pem");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        free(response.string);
        return NULL;
    }
    return response.string;
}
