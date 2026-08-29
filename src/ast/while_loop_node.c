#include "include/ast/while_loop_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>


ast_node_t* while_loop_node_new(ast_node_t *condition, ast_node_t *while_body, ast_node_t *else_body)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_WHILE_LOOP;
    node->node.while_loop = (while_loop_node_t*)malloc(sizeof(while_loop_node_t));
    node->node.while_loop->condition = condition;
    node->node.while_loop->while_body = while_body;
    node->node.while_loop->else_body = else_body;
    return node;
}

void while_loop_node_free(while_loop_node_t* while_loop_node)
{
    free(while_loop_node->condition);
    free(while_loop_node->while_body);
    free(while_loop_node->else_body);
    free(while_loop_node);
}

value_t while_loop_node_evaluate(const while_loop_node_t* while_loop_node, context_t* context)
{
    context_t* local_context = context_new(context);

    auto result = value_new_number(0);

    bool is_stopped = false;

    while (value_get_boolean(ast_node_evaluate(while_loop_node->condition, local_context)))
    {
        result = ast_node_evaluate(while_loop_node->while_body, local_context);


        switch (result.control_flow)
        {
            case CONTROL_FLOW_BREAK:
                result.control_flow = CONTROL_FLOW_NONE;
                is_stopped = true;
                break;
            case CONTROL_FLOW_THROW: return result;
            default: break;
        }
    }

    context_free(local_context);

    if (!is_stopped && while_loop_node->else_body)
    {
        local_context = context_new(context);
        result = ast_node_evaluate(while_loop_node->else_body, local_context);
        context_free(local_context);
    }

    return result;
}
