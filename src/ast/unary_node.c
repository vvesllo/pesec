#include "include/ast/node.h"
#include "include/common/panic.h"

pesec_node_t* pesec_unary_node_new(const pesec_unary_node_operation_t operation, pesec_node_t* operand)
{
    const auto unary_node = (pesec_unary_node_t){
        .operation = operation,
        .operand = operand
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_UNARY,
        (pesec_node_data_t){
            .unary_node = unary_node
        }
    );
}

pesec_value_t pesec_unary_node_evaluate(const pesec_unary_node_t* unary_node, pesec_environment_t* environment)
{
    const pesec_value_t right_value = pesec_node_evaluate(unary_node->operand, environment);

    switch (unary_node->operation)
    {
        case PESEC_UNARY_NODE_OPERATION_NEGATIVE: return pesec_value_negative(right_value);
    }

    pesec_panic("unary_node::evaluate: unknown operation (%d)", unary_node->operation);
}

void pesec_unary_node_destroy(const pesec_unary_node_t* unary_node)
{
    pesec_node_destroy(unary_node->operand);
}
