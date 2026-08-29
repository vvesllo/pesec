#ifndef PESEC_ARRAY_VALUE_H
#define PESEC_ARRAY_VALUE_H

#include "context.h"
#include "utils/typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct STATEMENT_SEQUENCE_NODE_STRUCT statement_sequence_node_t;

typedef struct ARRAY_VALUE_STRUCT
{
    ull_t size;
    ull_t capacity;
    value_t* values;
} array_value_t;

array_value_t* array_value_new(value_t* values, ull_t size);

value_t array_value_get(const array_value_t* array_value, ull_t index);

void array_value_set(const array_value_t* array_value, ull_t index, value_t value);

array_value_t* array_value_concat(const array_value_t* left, const array_value_t* right);

void array_value_free(array_value_t* array_value);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_ARRAY_VALUE_H
