#ifndef PESEC_UNARY_OP_NODE_H
#define PESEC_UNARY_OP_NODE_H

#include "include/token.h"
#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    token_t operation;
    ast_node_t* right;
} unary_op_node_t;

ast_node_t* unary_op_node_new(token_t operation, ast_node_t* right);

void unary_op_node_free(unary_op_node_t* unary_op_node);

value_t unary_op_node_evaluate(const unary_op_node_t* unary_op_node, context_t* context);

#endif // PESEC_UNARY_OP_NODE_H
