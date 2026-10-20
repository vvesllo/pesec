#include "include/ast/node.h"
#include "include/common/panic.h"

pesec_node_t* pesec_binary_node_new(pesec_binary_node_operation_t operation, pesec_node_t* left, pesec_node_t* right)
{
    const auto binary_node = (pesec_binary_node_t){
        .operation = operation,
        .left = left,
        .right = right
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_BINARY,
        (pesec_node_data_t){
            .binary_node = binary_node
        }
    );
}

pesec_value_t pesec_binary_node_evaluate(const pesec_binary_node_t* binary_node, pesec_environment_t* environment)
{
    const pesec_value_t left_value = pesec_node_evaluate(binary_node->left, environment);
    const pesec_value_t right_value = pesec_node_evaluate(binary_node->right, environment);

    switch (binary_node->operation)
    {
        case PESEC_BINARY_NODE_OPERATION_ADD: return pesec_value_add(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_SUB: return pesec_value_sub(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_MUL: return pesec_value_mul(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_DIV: return pesec_value_div(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_MOD: return pesec_value_mod(left_value, right_value);

        case PESEC_BINARY_NODE_OPERATION_EQUAL: return pesec_value_equal(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_LESS: return pesec_value_less(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_LESS_EQUAL: return pesec_value_less_equal(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_GREATER: return pesec_value_greater(left_value, right_value);
        case PESEC_BINARY_NODE_OPERATION_GREATER_EQUAL: return pesec_value_greater_equal(left_value, right_value);
    }

    pesec_panic("binary_node::evaluate: unknown operation (%d)", binary_node->operation);
}

void pesec_binary_node_destroy(const pesec_binary_node_t* binary_node)
{
    pesec_node_destroy(binary_node->left);
    pesec_node_destroy(binary_node->right);
}
