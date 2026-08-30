#ifndef PESEC_STRING_VALUE_H
#define PESEC_STRING_VALUE_H

#include "utils/typedefs.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct STRING_VALUE_STRUCT
{
    char* data;
    ull_t size;
    ull_t capacity;
} string_value_t;

string_value_t* string_value_new();

string_value_t* string_value_from(char* data, ull_t size);

string_value_t* string_value_from_cstr(const char* data);

void string_value_push_back(string_value_t* string, char data);

bool string_value_equals(const string_value_t* left, const string_value_t* right);

string_value_t* string_value_concat(const string_value_t* left, const string_value_t* right);

void string_value_free(string_value_t* string);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_STRING_VALUE_H