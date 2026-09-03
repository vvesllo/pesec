#ifndef PESEC_VECTOR_VALUE_H
#define PESEC_VECTOR_VALUE_H

#include "context.h"
#include "utils/typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define VECTOR_METHOD(vector_value_method, ...) value_new_function( \
    function_value_new( \
        parameter_new_from_cstr((const char*[]){__VA_ARGS__, nullptr}), \
        (function_value_value_t){ \
            .as_bound_method = { \
                .self = vector_value, \
                .method = vector_value_method \
            } \
        }, \
        FUNCTION_VALUE_TYPE_BOUND_METHOD, \
        context \
    ) \
);

#define VECTOR_METHOD_0(vector_value_method) value_new_function( \
    function_value_new( \
        parameter_new(), \
        (function_value_value_t){ \
            .as_bound_method = { \
                .self = vector_value, \
                .method = vector_value_method \
            } \
        }, \
        FUNCTION_VALUE_TYPE_BOUND_METHOD, \
        context \
    ) \
);


typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct STATEMENT_SEQUENCE_NODE_STRUCT statement_sequence_node_t;

typedef struct VECTOR_VALUE_STRUCT
{
    ull_t size;
    ull_t capacity;
    value_t* values;
} vector_value_t;

vector_value_t* vector_value_new(value_t* values, ull_t size);

vector_value_t* vector_value_copy(const vector_value_t* source);

void vector_value_free(vector_value_t* vector_value);

value_t vector_value_resolve_field(value_t vector_value, string_view_t name, context_t* context);

void vector_value_set(const vector_value_t* vector_value, long long index, value_t value);

value_t vector_value_get(const vector_value_t* vector_value, long long index);

void vector_value_push(vector_value_t* vector_value, value_t value);

value_t vector_value_pop(vector_value_t* vector_value);

vector_value_t* vector_value_concat(const vector_value_t* left, const vector_value_t* right);

long long vector_value_index_of(const vector_value_t* vector_value, value_t value);

bool vector_value_contains(const vector_value_t* vector_value, value_t value);

void vector_value_reverse(const vector_value_t* vector_value);

void vector_value_clear(vector_value_t* vector_value);

// ================================================================================

value_t vector_value_method_size(value_t vector_value, context_t* context);

value_t vector_value_method_copy(value_t vector_value, context_t* context);

value_t vector_value_method_push(value_t vector_value, context_t* context);

value_t vector_value_method_pop(value_t vector_value, context_t* context);

value_t vector_value_method_concat(value_t vector_value, context_t* context);

value_t vector_value_method_index_of(value_t vector_value, context_t* context);

value_t vector_value_method_contains(value_t vector_value, context_t* context);

value_t vector_value_method_reverse(value_t vector_value, context_t* context);

value_t vector_value_method_clear(value_t vector_value, context_t* context);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_VECTOR_VALUE_H
