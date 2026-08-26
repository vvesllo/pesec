#include "../include/lib.h"
#include "../../../include/function_value.h"
#include "../../../include/string_value.h"
#include "../../../include/value.h"
#include "../../../include/utils/throw.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    char* data;
    size_t size;
    size_t capacity;
} curl_response_t;

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    auto mem = (curl_response_t*)userp;

    if (mem->size + realsize + 1 > mem->capacity) {
        mem->capacity = (mem->capacity == 0) ? 1024 : mem->capacity * 2;
        while (mem->capacity < mem->size + realsize + 1) {
            mem->capacity *= 2;
        }
        char* ptr = realloc(mem->data, mem->capacity);
        if (!ptr) return 0;
        mem->data = ptr;
    }

    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

static value_t http_get(context_t* context)
{
    context_item_t* url_item = context_get_local(context, string_view_from("url"));
    if (url_item->value.type != VALUE_TYPE_STRING)
        THROW("http_get requires a string argument 'url'\n");

    const char* url = url_item->value.data.as_string->data;

    CURL* curl = curl_easy_init();
    if (!curl)
        THROW("Failed to initialize libcurl\n");

    curl_response_t response = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "curl_easy_perform() failed: %s", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(response.data);
        THROW("%s\n", error_msg);
    }

    curl_easy_cleanup(curl);

    if (!response.data)
    {
        response.data = calloc(1, 1);
        response.size = 0;
    }

    string_value_t* result_str = string_value_from(response.data, response.size);
    return value_new_string(result_str);
}

static value_t http_post(context_t* context)
{
    context_item_t* url_item = context_get_local(context, string_view_from("url"));
    context_item_t* body_item = context_get_local(context, string_view_from("body"));

    if (url_item->value.type != VALUE_TYPE_STRING)
        THROW("http_post requires a string argument 'url'\n");

    if (body_item->value.type != VALUE_TYPE_STRING)
        THROW("http_post requires a string argument 'body'\n");


    const char* url = url_item->value.data.as_string->data;
    const char* body = body_item->value.data.as_string->data;

    CURL* curl = curl_easy_init();
    if (!curl)
        THROW("Failed to initialize libcurl\n");

    curl_response_t response = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);

    if (res != CURLE_OK)
    {
        char error_msg[512];
        snprintf(error_msg, sizeof(error_msg), "curl_easy_perform() failed: %s", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(response.data);
        THROW("%s\n", error_msg);
    }

    curl_easy_cleanup(curl);

    if (!response.data) {
        response.data = calloc(1, 1);
        response.size = 0;
    }

    string_value_t* result_str = string_value_from(response.data, response.size);
    return value_new_string(result_str);
}

#define REG_FUNC(function_name, ...) \
    context_push( \
        context, \
        string_view_from(#function_name), \
        MAKE_VAL_FUNC( \
            function_value_new( \
                parameter_new_from_cstr((const char *[]){__VA_ARGS__, nullptr}), \
                (function_value_value_t){ \
                    .as_c_function = function_name \
                }, \
                FUNCTION_VALUE_TYPE_C_FUNCTION, \
                context \
            ) \
        ), \
        true \
    )

void init_lib(context_t* context) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    REG_FUNC(http_get, "url");
    REG_FUNC(http_post, "url", "body");
}