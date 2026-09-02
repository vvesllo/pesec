#ifndef PESEC_PANIC_NODE_H
#define PESEC_PANIC_NODE_H

#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* expression;
} panic_node_t;

ast_node_t* panic_node_new(ast_node_t* expression);

void panic_node_free(panic_node_t* panic_node);

value_t panic_node_evaluate(const panic_node_t* panic_node, context_t* context);

#endif // PESEC_PANIC_NODE_H
