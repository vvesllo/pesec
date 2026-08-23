#ifndef PESEC_VARIABLE_FIELD_ACCESS_NODE_H
#define PESEC_VARIABLE_FIELD_ACCESS_NODE_H

#include "include/value.h"
#include "include/context.h"
#include "include/parameter.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* object;
    string_view_t field;
} variable_field_access_node_t;

ast_node_t* variable_field_access_node_new(ast_node_t* object, string_view_t field);

void variable_field_access_node_free(variable_field_access_node_t* variable_field_node);

value_t variable_field_access_node_evaluate(const variable_field_access_node_t* variable_field_node, context_t* context);

#endif // PESEC_VARIABLE_FIELD_ACCESS_NODE_H
