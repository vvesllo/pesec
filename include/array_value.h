#ifndef PESEC_ARRAY_VALUE_H
#define PESEC_ARRAY_VALUE_H

#include "context.h"
#include "utils/typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define ARRAY_METHOD(array_value_method, ...) value_new_function( \
    function_value_new( \
        parameter_new_from_cstr((const char*[]){__VA_ARGS__, nullptr}), \
        (function_value_value_t){ \
            .as_bound_method = { \
                .self = array_value, \
                .method = array_value_method \
            } \
        }, \
        FUNCTION_VALUE_TYPE_BOUND_METHOD, \
        context \
    ) \
);

#define ARRAY_METHOD_0(array_value_method) value_new_function( \
    function_value_new( \
        parameter_new(), \
        (function_value_value_t){ \
            .as_bound_method = { \
                .self = array_value, \
                .method = array_value_method \
            } \
        }, \
        FUNCTION_VALUE_TYPE_BOUND_METHOD, \
        context \
    ) \
);


typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct STATEMENT_SEQUENCE_NODE_STRUCT statement_sequence_node_t;

typedef struct ARRAY_VALUE_STRUCT
{
    ull_t size;
    ull_t capacity;
    value_t* values;
} array_value_t;

array_value_t* array_value_new(value_t* values, ull_t size);

array_value_t* array_value_copy(const array_value_t* source);

void array_value_free(array_value_t* array_value);

value_t array_value_resolve_field(value_t array_value, string_view_t name, context_t* context);

void array_value_set(const array_value_t* array_value, long long index, value_t value);

value_t array_value_get(const array_value_t* array_value, long long index);

void array_value_push(array_value_t* array_value, value_t value);

value_t array_value_pop(array_value_t* array_value);

array_value_t* array_value_concat(const array_value_t* left, const array_value_t* right);

long long array_value_index_of(const array_value_t* array_value, value_t value);

bool array_value_contains(const array_value_t* array_value, value_t value);

void array_value_reverse(const array_value_t* array_value);

void array_value_clear(array_value_t* array_value);

// ================================================================================

value_t array_value_method_size(value_t array_value, context_t* context);

value_t array_value_method_copy(value_t array_value, context_t* context);

value_t array_value_method_push(value_t array_value, context_t* context);

value_t array_value_method_pop(value_t array_value, context_t* context);

value_t array_value_method_concat(value_t array_value, context_t* context);

value_t array_value_method_index_of(value_t array_value, context_t* context);

value_t array_value_method_contains(value_t array_value, context_t* context);

value_t array_value_method_reverse(value_t array_value, context_t* context);

value_t array_value_method_clear(value_t array_value, context_t* context);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_ARRAY_VALUE_H
