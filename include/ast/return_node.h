#ifndef PESEC_RETURN_NODE_H
#define PESEC_RETURN_NODE_H

#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* expression;
} return_node_t;

ast_node_t* return_node_new(ast_node_t* expression);

void return_node_free(return_node_t* return_node);

value_t return_node_evaluate(const return_node_t* return_node, context_t* context);

#endif // PESEC_RETURN_NODE_H
