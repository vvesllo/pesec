#include "include/ast/condition_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>


ast_node_t* condition_node_new(ast_node_t *condition, ast_node_t *if_body, ast_node_t *else_body)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_CONDITION;
    node->node.condition = (condition_node_t*)malloc(sizeof(condition_node_t));
    node->node.condition->condition = condition;
    node->node.condition->if_body = if_body;
    node->node.condition->else_body = else_body;
    return node;
}

void condition_node_free(condition_node_t* condition_node)
{
    free(condition_node->condition);
    free(condition_node->if_body);
    free(condition_node->else_body);
    free(condition_node);
}

value_t condition_node_evaluate(const condition_node_t* condition_node, context_t* context)
{
    const value_t value = ast_node_evaluate(condition_node->condition, context);
    const bool value_bool = value_get_boolean(value);
    if (value_bool) return ast_node_evaluate(condition_node->if_body, context);
    if (condition_node->else_body) return ast_node_evaluate(condition_node->else_body, context);
    return value_new_null();
}
