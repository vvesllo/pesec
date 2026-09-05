#include "include/ast/panic_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

ast_node_t* panic_node_new(ast_node_t* expression)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_PANIC;
    node->node.panic = (panic_node_t*)malloc(sizeof(panic_node_t));
    node->node.panic->expression = expression;
    return node;
}

void panic_node_free(panic_node_t* panic_node)
{
    if (!panic_node) return;
    if (panic_node->expression) ast_node_free(panic_node->expression);
    free(panic_node);
}

value_t panic_node_evaluate(const panic_node_t* panic_node, context_t* context)
{
    value_t result = ast_node_evaluate(panic_node->expression, context);
    result.control_flow = CONTROL_FLOW_PANIC;
    return result;
}
