#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_call_node_new(pesec_node_t* callee, pesec_node_vector_t args)
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

pesec_value_t pesec_call_node_evaluate(
    const pesec_call_node_t* call_node,
    pesec_environment_t* environment
)
{
    const pesec_value_t function = pesec_node_evaluate(call_node->callee, environment);

    if (function.type != PESEC_VALUE_TYPE_FUNCTION)
    {
        if (call_node->callee->type == PESEC_NODE_TYPE_VARIABLE)
            pesec_panic("call_node::evaluate: variable `%.*s` is not a function",
                        call_node->callee->data.variable_node.name.size,
                        call_node->callee->data.variable_node.name.data
            );
        pesec_panic("call_node::evaluate: can call function");
    }

    const size_t argc = call_node->args.size;
    const auto argv = (pesec_value_t*)pesec_calloc(argc, sizeof(pesec_value_t));

    pesec_environment_t local_environment = pesec_environment_new(environment);

    for (size_t i = 0; i < call_node->args.size; i++)
    {
        argv[i] = pesec_node_evaluate(call_node->args.items[i], &local_environment);
    }

    const pesec_value_t result_value = pesec_function_value_call(&function.data.function, argc, argv);

    pesec_environment_destroy(&local_environment);

    return result_value;
}

void pesec_call_node_destroy(const pesec_call_node_t* call_node)
{
    // nothing to do
}
