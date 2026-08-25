#include "include/function_value.h"

#include <stdlib.h>

#include "include/ast/ast_node.h"

function_value_t* function_value_new(parameter_t* parameter, const function_value_value_t body, const function_value_type_t type, context_t* parent_context)
{
    const auto function_value = (function_value_t*) malloc(sizeof(function_value_t));

    function_value->parameter = parameter;
    function_value->type = type;
    function_value->body = body;
    function_value->parent_context = parent_context;

    return function_value;
}

void function_value_free(function_value_t* function_value)
{
    if (function_value->type == FUNCTION_VALUE_TYPE_NODE)
        ast_node_free(function_value->body.as_node);
    free(function_value);
}

value_t function_value_call(const function_value_t* function_value, context_t* context)
{
    if (function_value->type == FUNCTION_VALUE_TYPE_NODE)
        return ast_node_evaluate(function_value->body.as_node, context);
    return function_value->body.as_c_function(context);
}