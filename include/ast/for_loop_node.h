#ifndef PESEC_FOR_LOOP_NODE_H
#define PESEC_FOR_LOOP_NODE_H

#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    string_view_t iterator;
    ast_node_t *iterable;
    ast_node_t *for_body;
    ast_node_t *else_body;
} for_loop_node_t;

ast_node_t* for_loop_node_new(string_view_t iterator, ast_node_t *iterable, ast_node_t *for_body, ast_node_t *else_body);

void for_loop_node_free(for_loop_node_t* for_loop_node);

value_t for_loop_node_evaluate(const for_loop_node_t* for_loop_node, context_t* context);

#endif // PESEC_FOR_LOOP_NODE_H
