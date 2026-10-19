#include "include/runtime/function_value.h"

#include "include/ast/node.h"
#include "include/common/alloc.h"

pesec_function_value_t* pesec_function_value_new(
    const pesec_function_value_type_t type,
    const pesec_function_value_data_t data,
    const pesec_parameter_vector_t parameter_vector,
    pesec_environment_t* parent_environment
)
{
    const auto function_value = (pesec_function_value_t*)pesec_malloc(sizeof(pesec_function_value_t));

    function_value->type = type;
    function_value->data = data;
    function_value->parameter_vector = parameter_vector;
    function_value->parent_environment = parent_environment;

    return function_value;
}

pesec_value_t pesec_function_value_call(pesec_function_value_t* function_value, pesec_environment_t* environment)
{
    if (function_value->type == PESEC_FUNCTION_VALUE_TYPE_NATIVE)
        return function_value->data.native(environment);
    if (function_value->type == PESEC_FUNCTION_VALUE_TYPE_METHOD)
        return function_value->data.method.method(&function_value->data.method.self, environment);

    return pesec_node_evaluate(function_value->data.node, environment);
}

void pesec_function_value_destroy(pesec_function_value_t* function_value)
{
    pesec_free(function_value);
}
