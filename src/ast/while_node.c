#include "include/ast/node.h"
#include "include/runtime/environment.h"
#include "include/runtime/vector_value.h"

pesec_node_t* pesec_while_node_new(pesec_node_t* condition, pesec_node_t* body)
{
    const auto while_node = (pesec_while_node_t){
        .condition = condition,
        .body = body,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_WHILE,
        (pesec_node_data_t){
            .while_node = while_node
        }
    );
}

pesec_value_t pesec_while_node_evaluate(
    const pesec_while_node_t* while_node,
    pesec_environment_t* environment
)
{
    pesec_vector_value_t* result_vector_value = pesec_vector_value_new();
    pesec_value_t value = pesec_value_as_none();
    pesec_environment_t* local_environment = pesec_environment_new(environment);
    while (pesec_value_is_truthy(pesec_node_evaluate(while_node->condition, local_environment)))
    {
        value = pesec_node_evaluate(while_node->body, local_environment);
        if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_CONTINUE)
        {
            value.control_flow = PESEC_VALUE_CONTROL_FLOW_NONE;
            if (value.type != PESEC_VALUE_TYPE_NONE) pesec_vector_value_push(result_vector_value, value);
            continue;
        }
        if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_BREAK)
        {
            value.control_flow = PESEC_VALUE_CONTROL_FLOW_NONE;
            if (value.type != PESEC_VALUE_TYPE_NONE) pesec_vector_value_push(result_vector_value, value);
            break;
        }
        if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_RETURN) break;

        pesec_variable_table_reset(local_environment->variable_table);
    }

    pesec_environment_destroy(local_environment);

    if (value.control_flow == PESEC_VALUE_CONTROL_FLOW_BREAK ||
        value.control_flow == PESEC_VALUE_CONTROL_FLOW_RETURN)
    {
        return value;
    }

    return pesec_value_as_vector(result_vector_value);
}

void pesec_while_node_destroy(const pesec_while_node_t* while_node)
{
    pesec_node_destroy(while_node->condition);
    pesec_node_destroy(while_node->body);
}
