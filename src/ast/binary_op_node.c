#include "include/ast/binary_op_node.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/ast/ast_node.h"
#include "include/utils/throw.h"

ast_node_t* binary_op_node_new(const token_t operation, ast_node_t* left, ast_node_t* right)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_BINARY;
    node->node.binary_op = (binary_op_node_t*)malloc(sizeof(binary_op_node_t));
    node->node.binary_op->operation = operation;
    node->node.binary_op->left = left;
    node->node.binary_op->right = right;
    return node;
}

void binary_op_node_free(binary_op_node_t* binary_op_node)
{
    ast_node_free(binary_op_node->left);
    ast_node_free(binary_op_node->right);
    free(binary_op_node);
}

value_t binary_op_node_evaluate(const binary_op_node_t* binary_op_node, context_t* context)
{
    const value_t left_value = ast_node_evaluate(binary_op_node->left, context);
    const value_t right_value = ast_node_evaluate(binary_op_node->right, context);

    switch (binary_op_node->operation.type)
    {
        case TOKEN_TYPE_PLUS: return value_operation_add(left_value, right_value);
        case TOKEN_TYPE_MINUS: return value_operation_sub(left_value, right_value);
        case TOKEN_TYPE_SLASH: return value_operation_div(left_value, right_value);
        case TOKEN_TYPE_SLASH_SLASH: return value_operation_floor_div(left_value, right_value);
        case TOKEN_TYPE_ASTERISK: return value_operation_mul(left_value, right_value);
        case TOKEN_TYPE_ASTERISK_ASTERISK: return value_operation_pow(left_value, right_value);


        case TOKEN_TYPE_EQUALS_EQUALS: return value_operation_equals(left_value, right_value);
        case TOKEN_TYPE_LESS_EQUALS: return value_operation_less_or_equals(left_value, right_value);
        case TOKEN_TYPE_LESS: return value_operation_less(left_value, right_value);
        case TOKEN_TYPE_GREATER_EQUALS: return value_operation_greater_or_equals(left_value, right_value);
        case TOKEN_TYPE_GREATER: return value_operation_greater(left_value, right_value);
        default:
            THROW("Unknown binary operator type %d\n", binary_op_node->operation.type);
    }
}
