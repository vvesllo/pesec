#include "include/runtime/function_value.h"

#include "include/ast/node.h"

pesec_value_t pesec_function_value_call(const pesec_function_value_t* function_value, pesec_environment_t* environment)
{
    if (function_value->type == PESEC_FUNCTION_VALUE_TYPE_NATIVE)
        return function_value->data.native(environment);

    return pesec_node_evaluate(function_value->data.node, environment);
}
