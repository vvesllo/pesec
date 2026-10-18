#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_call_node_new(pesec_node_t* callee, const pesec_node_vector_t args)
{
    const auto call_node = (pesec_call_node_t){
        .callee = callee,
        .args = args,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_CALL,
        (pesec_node_data_t){
            .call_node = call_node
        }
    );
}

pesec_value_t pesec_call_node_evaluate(const pesec_call_node_t* call_node, pesec_environment_t* environment)
{
    const pesec_value_t value = pesec_node_evaluate(call_node->callee, environment);

    if (value.type != PESEC_VALUE_TYPE_FUNCTION)
    {
        if (call_node->callee->type == PESEC_NODE_TYPE_VARIABLE)
            pesec_panic("call_node::evaluate: variable `%.*s` is not a function",
                        call_node->callee->data.variable_node.name.size,
                        call_node->callee->data.variable_node.name.data
            );
        pesec_panic("call_node::evaluate: can call function");
    }

    const pesec_function_value_t function = value.data.function;

    if (function.parameter_vector.size != call_node->args.size)
        pesec_panic("function::call: invalid function argument count (%d but %d given)",
                    function.parameter_vector.size, call_node->args.size);

    pesec_environment_t local_environment = pesec_environment_new(function.parent_environment);

    for (size_t i = 0; i < call_node->args.size; i++)
    {
        const pesec_value_t arg_value = pesec_node_evaluate(call_node->args.items[i], &local_environment);
        pesec_environment_push_variable(
            &local_environment,
            function.parameter_vector.items[i].name,
            arg_value,
            false
        );
    }

    const pesec_value_t result_value = pesec_function_value_call(&function);

    pesec_environment_destroy(&local_environment);

    return result_value;
}

void pesec_call_node_destroy(const pesec_call_node_t* call_node)
{
    // nothing to do
}
