#ifndef PESEC_STRING_VALUE_H
#define PESEC_STRING_VALUE_H

#include "context.h"
#include "utils/typedefs.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define STRING_METHOD(string_value_method, ...) value_new_function( \
    function_value_new( \
        parameter_new_from_cstr((const char*[]){__VA_ARGS__, nullptr}), \
        (function_value_value_t){ \
            .as_bound_method = { \
                .self = string_value, \
                .method = string_value_method \
            } \
        }, \
        FUNCTION_VALUE_TYPE_BOUND_METHOD, \
        context \
    ) \
);

#define STRING_METHOD_0(string_value_method) value_new_function( \
    function_value_new( \
        parameter_new(), \
        (function_value_value_t){ \
            .as_bound_method = { \
                .self = string_value, \
                .method = string_value_method \
            } \
        }, \
        FUNCTION_VALUE_TYPE_BOUND_METHOD, \
        context \
    ) \
);

typedef struct VALUE_STRUCT value_t;

typedef struct STRING_VALUE_STRUCT
{
    char* data;
    ull_t size;
    ull_t capacity;
} string_value_t;

string_value_t* string_value_new();

string_value_t* string_value_from(char* data, ull_t size);

string_value_t* string_value_from_cstr(const char* data);

string_value_t* string_value_from_string_view(string_view_t string_view);

void string_value_free(string_value_t* string);

void string_value_push_back(string_value_t* string, char data);

bool string_value_equals(const string_value_t* left, const string_value_t* right);

string_value_t* string_value_concat(const string_value_t* left, const string_value_t* right);

value_t string_value_get_fields(const string_value_t* string_value);


value_t string_value_resolve_field(value_t string_value, string_view_t name, context_t* context);

value_t string_value_method_size(value_t string_value, context_t* context);

value_t string_value_method_to_vector(value_t string_value, context_t* context);

value_t string_value_method_join(value_t string_value, context_t* context);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_STRING_VALUE_H