#ifndef PESEC_BINARY_OP_NODE_H
#define PESEC_BINARY_OP_NODE_H

#include "include/token.h"
#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    token_t operation;
    ast_node_t* left;
    ast_node_t* right;
} binary_op_node_t;

ast_node_t* binary_op_node_new(token_t operation, ast_node_t* left, ast_node_t* right);

void binary_op_node_free(binary_op_node_t* binary_op_node);

value_t binary_op_node_evaluate(const binary_op_node_t* binary_op_node, context_t* context);

#endif // PESEC_BINARY_OP_NODE_H
