#ifndef PESEC_STRUCTURE_DEFINITION_NODE_H
#define PESEC_STRUCTURE_DEFINITION_NODE_H

#include "include/value.h"
#include "include/context.h"
#include "include/parameter.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* fields;
} structure_definition_node_t;

ast_node_t* structure_definition_node_new(ast_node_t* fields);

void structure_definition_node_free(structure_definition_node_t* structure_definition_node);

value_t structure_definition_node_evaluate(const structure_definition_node_t* structure_definition_node, context_t* context);

#endif // PESEC_STRUCTURE_DEFINITION_NODE_H
