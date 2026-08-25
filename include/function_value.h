#ifndef PESEC_FUNCTION_VALUE_H
#define PESEC_FUNCTION_VALUE_H
#include "context.h"
#include "parameter.h"
#include "value.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef enum
{
    FUNCTION_VALUE_TYPE_NODE,
    FUNCTION_VALUE_TYPE_C_FUNCTION,
} function_value_type_t;

typedef union
{
    ast_node_t* as_node;
    value_t (*as_c_function)(context_t* context);
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

#endif // PESEC_FUNCTION_VALUE_H
