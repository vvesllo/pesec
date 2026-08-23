#include "include/ast/variable_definition_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>


ast_node_t* variable_definition_node_new(const string_view_t name, ast_node_t* value, const bool constant)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VARIABLE_DEFINITION;
    node->node.variable_definition = (variable_definition_node_t*)malloc(sizeof(variable_definition_node_t));
    node->node.variable_definition->name = name;
    node->node.variable_definition->value = value;
    node->node.variable_definition->constant = constant;
    return node;
}

void variable_definition_node_free(variable_definition_node_t* variable_definition_node)
{
    free(variable_definition_node);
}

value_t variable_definition_node_evaluate(const variable_definition_node_t* variable_definition_node, context_t* context)
{
    const value_t value = ast_node_evaluate(variable_definition_node->value, context);
    context_push(context, variable_definition_node->name, value, variable_definition_node->constant);
    return value;
}
