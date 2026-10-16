#include "include/ast/node.h"

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

pesec_value_t pesec_binary_node_evaluate(const pesec_binary_node_t* binary_node)
{
    const pesec_value_t left_value = pesec_node_evaluate(binary_node->left);
    const pesec_value_t right_value = pesec_node_evaluate(binary_node->right);

    switch (binary_node->operation)
    {
        case PESEC_BINARY_NODE_OPERATION_ADD: return left_value + right_value;
        case PESEC_BINARY_NODE_OPERATION_SUB: return left_value - right_value;
        case PESEC_BINARY_NODE_OPERATION_MUL: return left_value * right_value;
        case PESEC_BINARY_NODE_OPERATION_DIV: return left_value / right_value;
    }

    return 0;
}

void pesec_binary_node_destroy(const pesec_binary_node_t* binary_node)
{
    pesec_node_destroy(binary_node->left);
    pesec_node_destroy(binary_node->right);
}
