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
    _socBuffer = (u32 *)memalign(0x1000, 0x100000);
    socInit(_socBuffer, 0x100000);
    return 0;
}

int exitInternet() {
    if (_socBuffer) {
        free(_socBuffer);
        _socBuffer = NULL;
    }
    return 0;
}

size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;
    Response *response = (Response *)userdata;

    char *ptr = realloc(response->string, response->size + real_size + 1);
    if (ptr == NULL)
        return CURL_WRITEFUNC_ERROR;

    response->string = ptr;
    memcpy(&(response->string[response->size]), data, real_size);
    response->size += real_size;
    response->string[response->size] = '\0';
    return real_size;
}

char *miniCurlGet(const char *url) {
    Response response;
    response.string = calloc(1, 1);
    response.size = 0;

    CURL *curl;
    CURLcode result;

    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
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
    curl_easy_cleanup(curl);
    return response.string;
}
