#ifndef PESEC_THROW_NODE_H
#define PESEC_THROW_NODE_H

#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* expression;
} throw_node_t;

ast_node_t* throw_node_new(ast_node_t* expression);

void throw_node_free(throw_node_t* throw_node);

value_t throw_node_evaluate(const throw_node_t* throw_node, context_t* context);

#endif // PESEC_THROW_NODE_H
