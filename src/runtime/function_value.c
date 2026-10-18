#include "include/runtime/function_value.h"

#include "include/ast/node.h"

pesec_value_t pesec_function_value_call(const pesec_function_value_t* function_value)
{
    if (function_value->type == PESEC_FUNCTION_VALUE_TYPE_NATIVE)
        return function_value->data.native(function_value->parent_environment);

    return pesec_node_evaluate(function_value->data.node, function_value->parent_environment);
}
