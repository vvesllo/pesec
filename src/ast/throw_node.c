#include "include/ast/throw_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

ast_node_t* throw_node_new(ast_node_t* expression)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_THROW;
    node->node._throw = (throw_node_t*)malloc(sizeof(throw_node_t));
    node->node._throw->expression = expression;
    return node;
}

void throw_node_free(throw_node_t* throw_node)
{
    free(throw_node);
}

value_t throw_node_evaluate(const throw_node_t* throw_node, context_t* context)
{
    value_t result = ast_node_evaluate(throw_node->expression, context);
    result.control_flow = CONTROL_FLOW_THROW;
    return result;
}
