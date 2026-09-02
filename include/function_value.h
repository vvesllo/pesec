#ifndef PESEC_FUNCTION_VALUE_H
#define PESEC_FUNCTION_VALUE_H
#include "context.h"
#include "parameter.h"
#include "value.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct AST_NODE_STRUCT ast_node_t;

typedef enum
{
    FUNCTION_VALUE_TYPE_NODE,
    FUNCTION_VALUE_TYPE_C_FUNCTION,
    FUNCTION_VALUE_TYPE_BOUND_METHOD,
} function_value_type_t;

typedef struct BOUND_METHOD_DATA_STRUCT
{
    value_t self;
    value_t (*method)(value_t self, context_t* context);
} bound_method_data_t;

typedef union
{
    ast_node_t* as_node;
    value_t (*as_c_function)(context_t* context);
    bound_method_data_t as_bound_method;
} function_value_value_t;

typedef struct FUNCTION_VALUE_STRUCT
{
    parameter_t* parameter;
    function_value_type_t type;
    function_value_value_t body;
    context_t* parent_context;
} function_value_t;

function_value_t* function_value_new(parameter_t* parameter, function_value_value_t body, function_value_type_t type, context_t* parent_context);

void function_value_free(function_value_t* function_value);

value_t function_value_call(const function_value_t* function_value, context_t* context);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_FUNCTION_VALUE_H
