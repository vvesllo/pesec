#ifndef PESEC_VECTOR_ACCESS_NODE_H
#define PESEC_VECTOR_ACCESS_NODE_H

#include "statement_sequence_node.h"
#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* vector;
    ast_node_t* index;
} vector_access_node_t;

ast_node_t* vector_access_node_new(ast_node_t* vector, ast_node_t* index);

void vector_access_node_free(vector_access_node_t* vector_access_node);

value_t vector_access_node_evaluate(const vector_access_node_t* vector_access_node, context_t* context);

#endif // PESEC_VECTOR_ACCESS_NODE_H
