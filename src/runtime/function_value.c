#include "include/runtime/function_value.h"

#include "include/ast/node.h"
#include "include/common/panic.h"

pesec_value_t pesec_function_value_call(
    const pesec_function_value_t* function_value,
    const size_t argc,
    pesec_value_t* argv
)
{
    if (function_value->parameter_vector.size != argc)
        pesec_panic("function_value::call: invalid function argument count (%d but %d given)",
                    function_value->parameter_vector.size, argc);

    if (function_value->type == PESEC_FUNCTION_VALUE_TYPE_NATIVE)
        return function_value->data.native(argc, argv, function_value->parent_environment);

    return pesec_node_evaluate(function_value->data.node, function_value->parent_environment);
}
