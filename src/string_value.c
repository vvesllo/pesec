#include "../include/string_value.h"
#include "../include/vector_value.h"
#include "../include/function_value.h"

#include <stdlib.h>
#include <string.h>

#include "include/number_value.h"
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

string_value_t* string_value_from_string_view(string_view_t string_view)
{
    string_value_t* string = string_value_new();
    for (ull_t i = 0; i < string_view.length; i++)
        string_value_push_back(string, string_view.data[i]);
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

value_t string_value_get_fields(const string_value_t* string_value)
{
    vector_value_t* fields_vector = vector_value_new_size(0);
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("size")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("to_vector")));
    vector_value_push(fields_vector, value_new_string(string_value_from_cstr("join")));
    return value_new_vector(fields_vector);
}


value_t string_value_resolve_field(const value_t string_value, const string_view_t name, context_t* context)
{
    if (string_view_equals_cstr(name, "size")) return STRING_METHOD_0(string_value_method_size);
    if (string_view_equals_cstr(name, "to_vector")) return STRING_METHOD_0(string_value_method_to_vector);
    if (string_view_equals_cstr(name, "join")) return STRING_METHOD(string_value_method_join, "vector");

    THROW("Variable '%s' does not have a field", name.data);
}

value_t string_value_method_size(const value_t string_value, context_t* context)
{
    return value_new_number(number_value_from_long_double(string_value.data.as_string->size));
}

value_t string_value_method_to_vector(const value_t string_value, context_t* context)
{
    const string_value_t* source = string_value.data.as_string;

    vector_value_t* vector_vector = vector_value_new(nullptr, 0);
    vector_vector->capacity = source->size > 0 ? source->size : 1;
    vector_vector->values = (value_t*)malloc(sizeof(value_t) * vector_vector->capacity);
    vector_vector->size = 0;

    for (ull_t i = 0; i < source->size; ++i)
    {
        const char char_str[2] = { source->data[i], '\0' };
        string_value_t* char_string = string_value_from_cstr(char_str);

        vector_value_push(vector_vector, value_new_string(char_string));
    }

    return value_new_vector(vector_vector);
}

value_t string_value_method_join(const value_t string_value, context_t* context)
{
    const context_item_t* vector_item = context_get(context, string_view_from("vector"));

    if (!vector_item || vector_item->value.type != VALUE_TYPE_VECTOR)
        THROW("join expects a vector argument");

    const string_value_t* separator = string_value.data.as_string;
    const vector_value_t* vector = vector_item->value.data.as_vector;

    string_value_t* result = string_value_new();

    for (ull_t i = 0; i < vector->size; ++i)
    {
        const value_t element = vector->values[i];

        if (element.type != VALUE_TYPE_STRING)
            THROW("All elements in the vector must be strings for join");

        const string_value_t* str_element = element.data.as_string;

        for (ull_t j = 0; j < str_element->size; ++j)
            string_value_push_back(result, str_element->data[j]);

        if (i < vector->size - 1)
        {
            for (ull_t j = 0; j < separator->size; ++j)
                string_value_push_back(result, separator->data[j]);
        }
    }

    return value_new_string(result);
}