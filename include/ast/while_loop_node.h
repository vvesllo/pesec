#ifndef PESEC_WHILE_LOOP_NODE_H
#define PESEC_WHILE_LOOP_NODE_H

#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t *condition;
    ast_node_t *while_body;
    ast_node_t *else_body;
} while_loop_node_t;

ast_node_t* while_loop_node_new(ast_node_t *condition, ast_node_t *while_body, ast_node_t *else_body);

void while_loop_node_free(while_loop_node_t* while_loop_node);

value_t while_loop_node_evaluate(const while_loop_node_t* while_loop_node, context_t* context);

#endif // PESEC_WHILE_LOOP_NODE_H
