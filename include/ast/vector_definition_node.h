#ifndef PESEC_VECTOR_DEFINITION_NODE_H
#define PESEC_VECTOR_DEFINITION_NODE_H

#include "statement_sequence_node.h"
#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* values;
} vector_definition_node_t;

ast_node_t* vector_definition_node_new(ast_node_t* values);

void vector_definition_node_free(vector_definition_node_t* vector_definition_node);

value_t vector_definition_node_evaluate(const vector_definition_node_t* vector_definition_node, context_t* context);

#endif // PESEC_VECTOR_DEFINITION_NODE_H
