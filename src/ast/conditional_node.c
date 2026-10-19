#include <stdio.h>

#include "include/ast/node.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_conditional_node_new(
    pesec_node_t* condition,
    pesec_node_t* then_body,
    pesec_node_t* else_body
)
{
    const auto conditional_node = (pesec_conditional_node_t){
        .condition = condition,
        .then_body = then_body,
        .else_body = else_body
    };

    return pesec_node_new(
        PESEC_NODE_TYPE_CONDITIONAL,
        (pesec_node_data_t){
            .conditional_node = conditional_node
        }
    );
}

pesec_value_t pesec_conditional_node_evaluate(
    const pesec_conditional_node_t* conditional_node,
    pesec_environment_t* environment
)
{
    const pesec_value_t condition_value = pesec_node_evaluate(conditional_node->condition, environment);

    if (pesec_value_is_truthy(condition_value))
        return pesec_node_evaluate(conditional_node->then_body, environment);

    if (conditional_node->else_body)
        return pesec_node_evaluate(conditional_node->else_body, environment);

    return pesec_value_none();
}

void pesec_conditional_node_destroy(const pesec_conditional_node_t* conditional_node)
{
    pesec_node_destroy(conditional_node->condition);
    pesec_node_destroy(conditional_node->then_body);
    if (conditional_node->else_body)
        pesec_node_destroy(conditional_node->else_body);
}
