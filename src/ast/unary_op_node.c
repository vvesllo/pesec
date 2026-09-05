#include "include/ast/unary_op_node.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/ast/ast_node.h"
#include "include/utils/throw.h"

ast_node_t* unary_op_node_new(const token_t operation, ast_node_t* right)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_UNARY;
    node->node.unary_op = (unary_op_node_t*)malloc(sizeof(unary_op_node_t));
    node->node.unary_op->operation = operation;
    node->node.unary_op->right = right;
    return node;
}

void unary_op_node_free(unary_op_node_t* unary_op_node)
{
    if (!unary_op_node) return;
    if (unary_op_node->right) ast_node_free(unary_op_node->right);
    free(unary_op_node);
}

value_t unary_op_node_evaluate(const unary_op_node_t* unary_op_node, context_t* context)
{
    const value_t right_value = ast_node_evaluate(unary_op_node->right, context);

    switch (unary_op_node->operation.type)
    {
        case TOKEN_TYPE_PLUS: return right_value;
        case TOKEN_TYPE_MINUS: return value_operation_negate(right_value);
        case TOKEN_TYPE_EXCLAMATION_MARK: return value_operation_not(right_value);
        default:
            THROW("Unknown unary operator type %d\n", unary_op_node->operation.type);
    }
}
