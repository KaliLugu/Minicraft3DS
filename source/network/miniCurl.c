#include "miniCurl.h"
#include "../version.h"

static u32 *_socBuffer = NULL;

typedef struct {
    char *string;
    size_t size;
} Response;


size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata);

int internetInit() {
    acInit();
    Result acResult = acWaitInternetConnection();
    if (R_FAILED(acResult)) {
        FILE *f = fopen("sdmc:/minicraft_net.txt", "w");
        if (f) { fprintf(f, "acWait failed: %ld\n", acResult); fclose(f); }
        acExit();
        return -1;
    }

    FILE *f = fopen("sdmc:/minicraft_net.txt", "w");
    if (f) { fprintf(f, "acWait OK, doing socInit\n"); fclose(f); }

    _socBuffer = (u32 *)memalign(0x1000, 0x100000);
    if (!_socBuffer) {
        acExit();
        return -1;
    }
    Result socResult = socInit(_socBuffer, 0x100000);
    if (R_FAILED(socResult)) {
        free(_socBuffer);
        _socBuffer = NULL;
        acExit();
        return -1;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    return 0;
}

int exitInternet() {
    curl_global_cleanup();
    socExit();
    if (_socBuffer) {
        free(_socBuffer);
        _socBuffer = NULL;
    }
    acExit();
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

    CURL *curl = curl_easy_init();
    if (!curl) {
        free(response.string);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Minicraft3DS/" VERSION_STRING);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "romfs:/cacert.pem");
    curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "8.8.8.8");

    CURLcode curlResult = curl_easy_perform(curl);
    if (curlResult != CURLE_OK) {
        FILE *f = fopen("sdmc:/minicraft_curl_err.txt", "w");
        if (f) { fprintf(f, "curl error %d: %s\n", curlResult, curl_easy_strerror(curlResult)); fclose(f); }
        curl_easy_cleanup(curl);
        free(response.string);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return response.string;
}
