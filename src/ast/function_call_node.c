#include "include/ast/node.h"
#include "include/common/panic.h"

pesec_node_t* pesec_function_call_node_new(const pesec_string_view_t name, pesec_node_vector_t args)
{
    const auto function_call_node = (pesec_function_call_node_t){
        .name = name,
        .args = args,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_FUNCTION_CALL,
        (pesec_node_data_t){
            .function_call_node = function_call_node
        }
    );
}

pesec_value_t pesec_function_call_node_evaluate(const pesec_function_call_node_t* function_call_node,
                                                pesec_environment_t* environment)
{
    if (pesec_string_view_equals_cstr(function_call_node->name, "add"))
    {
        if (function_call_node->args.size != 2)
            pesec_panic("function_call_node::evaluate: function `%.*s` takes %d positional arguments but %d were given",
                        function_call_node->name.size, function_call_node->name.data,
                        2,
                        function_call_node->args.size);

        const pesec_value_t left = pesec_node_evaluate(function_call_node->args.items[0], environment);
        const pesec_value_t right = pesec_node_evaluate(function_call_node->args.items[1], environment);

        return pesec_value_add(left, right);
    }

    return pesec_variable_table_get(
        &environment->variable_table,
        function_call_node->name
    )->value;
}

void pesec_function_call_node_destroy(const pesec_function_call_node_t* function_call_node)
{
    // nothing to do
}
