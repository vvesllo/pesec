#include "include/ast/return_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

ast_node_t* return_node_new(ast_node_t* expression)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_RETURN;
    node->node._return = (return_node_t*)malloc(sizeof(return_node_t));
    node->node._return->expression = expression;
    return node;
}

void return_node_free(return_node_t* return_node)
{
    ast_node_free(return_node->expression);
    free(return_node);
}

value_t return_node_evaluate(const return_node_t* return_node, context_t* context)
{
    if (!return_node->expression) return value_new_null();

    value_t result = ast_node_evaluate(return_node->expression, context);
    result.control_flow = CONTROL_FLOW_RETURN;
    return result;
}
