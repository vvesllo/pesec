#include "../include/string_value.h"

#include <stdlib.h>
#include <string.h>

#include "include/utils/throw.h"


string_value_t* string_value_new()
{
    const auto string = (string_value_t*)malloc(sizeof(string_value_t));

    string->capacity = 64;
    string->size = 0;
    string->data = (char*)malloc(string->capacity * sizeof(char));
    string->data[string->size] = '\0';

    return string;
}

string_value_t* string_value_from(char* data, const ull_t size)
{
    const auto string = (string_value_t*)malloc(sizeof(string_value_t));

    string->capacity = size;
    string->size = size;
    string->data = data;

    return string;
}

string_value_t* string_value_from_cstr(const char* data)
{
    string_value_t* string = string_value_new();
    for (ull_t i = 0; i < strlen(data); i++)
        string_value_push_back(string, data[i]);
    return string;
}

void string_value_push_back(string_value_t* string, const char data)
{
    if (string->size + 1 >= string->capacity)
    {
        string->capacity = string->capacity * 2;
        const auto temp = (char*)realloc(string->data, string->capacity * sizeof(char));
        if (!temp) THROW("Failed to realloc memory to string");
        string->data = temp;
    }

    string->data[string->size] = data;
    string->size++;

    string->data[string->size] = '\0';
}

bool string_value_equals(const string_value_t* left, const string_value_t* right)
{
    if (left->size != right->size) return false;

    return memcmp(left->data, right->data, left->size) == 0;
}

string_value_t* string_value_concat(const string_value_t* left, const string_value_t* right)
{
    string_value_t* string = string_value_new();

    for (ull_t i = 0; i < left->size; i++) string_value_push_back(string, left->data[i]);
    for (ull_t i = 0; i < right->size; i++) string_value_push_back(string, right->data[i]);

    return string;
}

void string_value_free(string_value_t* string)
{
    free(string->data);
    free(string);
}