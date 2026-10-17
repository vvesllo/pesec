#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"

pesec_node_t* pesec_call_node_new(const pesec_string_view_t name, pesec_node_vector_t args)
{
    const auto call_node = (pesec_call_node_t){
        .name = name,
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
    if (pesec_string_view_equals_cstr(call_node->name, "add"))
    {
        if (call_node->args.size != 2)
            pesec_panic("call_node::evaluate: function `%.*s` takes %d positional arguments but %d were given",
                        call_node->name.size, call_node->name.data,
                        2,
                        call_node->args.size);

        const pesec_value_t left = pesec_node_evaluate(call_node->args.items[0], environment);
        const pesec_value_t right = pesec_node_evaluate(call_node->args.items[1], environment);

        return pesec_value_add(left, right);
    }

    const pesec_variable_t* function = pesec_environment_get_variable(environment, call_node->name);

    if (function->value.type != PESEC_VALUE_TYPE_FUNCTION)
        pesec_panic("call_node::evaluate: variable `%.*s` should be a function", call_node->name.size,
                    call_node->name.data);

    const size_t argc = call_node->args.size;
    const auto argv = (pesec_value_t*)pesec_calloc(argc, sizeof(pesec_value_t));

    for (size_t i = 0; i < call_node->args.size; i++)
    {
        argv[i] = pesec_node_evaluate(call_node->args.items[i], environment);
    }

    return function->value.data.function(argc, argv, environment);
}

void pesec_call_node_destroy(const pesec_call_node_t* call_node)
{
    // nothing to do
}
