#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/vector_value.h"

pesec_node_t* pesec_for_node_new(const pesec_string_view_t iterator, pesec_node_t* iterable, pesec_node_t* body)
{
    const auto for_node = (pesec_for_node_t){
        .iterator = iterator,
        .iterable = iterable,
        .body = body,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_FOR,
        (pesec_node_data_t){
            .for_node = for_node
        }
    );
}

pesec_value_t pesec_for_node_evaluate(
    const pesec_for_node_t* for_node,
    pesec_environment_t* environment
)
{
    const pesec_value_t iterable_value = pesec_node_evaluate(for_node->iterable, environment);

    if (iterable_value.type != PESEC_VALUE_TYPE_VECTOR)
        pesec_panic("for_node::evaluate: iterable should be vector");

    const pesec_vector_value_t* iterable = iterable_value.data.vector;

    if (iterable->size > 0)
    {
        pesec_vector_value_t* result_vector_value = pesec_vector_value_new();

        pesec_value_t value = pesec_value_as_none();
        for (size_t i = 0; i < iterable->size; i++)
        {
            pesec_environment_t* local_environment = pesec_environment_new(environment);
            pesec_environment_push_variable(
                local_environment,
                for_node->iterator,
                iterable->items[i],
                false
            );
            value = pesec_node_evaluate(for_node->body, local_environment);
            if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_CONTINUE)
            {
                value.control_flow = PESEC_VALUE_CONTROL_FLOW_NONE;
                pesec_vector_value_push(result_vector_value, value);
                continue;
            }
            if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_BREAK) break;
            if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_RETURN) break;
            pesec_environment_destroy(local_environment);
        }


        if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_BREAK || value.control_flow ==
            PESEC_VALUE_CONTROL_FLOW_RETURN)
        {
            return value;
        }

        return pesec_value_as_vector(result_vector_value);
    }
    return pesec_value_as_none();
}

void pesec_for_node_destroy(const pesec_for_node_t* for_node)
{
    pesec_node_destroy(for_node->iterable);
    pesec_node_destroy(for_node->body);
}
