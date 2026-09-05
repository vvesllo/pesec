#include "include/ast/literal_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

ast_node_t* literal_node_new(const value_t value)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_LITERAL;
    node->node.literal = (literal_node_t*)malloc(sizeof(literal_node_t));
    node->node.literal->value = value;
    return node;
}

void literal_node_free(literal_node_t* literal_node)
{
    if (literal_node) free(literal_node);
}

value_t literal_node_evaluate(const literal_node_t* literal_node, context_t* context)
{
    return literal_node->value;
}
