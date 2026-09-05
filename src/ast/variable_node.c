#include "include/ast/literal_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>


ast_node_t* variable_node_new(const string_view_t name)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VARIABLE;
    node->node.variable = (variable_node_t*)malloc(sizeof(variable_node_t));
    node->node.variable->name = name;
    return node;
}

void variable_node_free(variable_node_t* variable_node)
{
    if (variable_node)
        free(variable_node);
}

value_t variable_node_evaluate(const variable_node_t* variable_node, const context_t* context)
{
    return context_get(context, variable_node->name)->value;
}
