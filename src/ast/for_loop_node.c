#include "include/ast/for_loop_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/vector_value.h"
#include "include/utils/throw.h"


ast_node_t* for_loop_node_new(const string_view_t iterator, ast_node_t *iterable, ast_node_t *for_body, ast_node_t *else_body)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_FOR_LOOP;
    node->node.for_loop = (for_loop_node_t*)malloc(sizeof(for_loop_node_t));
    node->node.for_loop->iterator = iterator;
    node->node.for_loop->iterable = iterable;
    node->node.for_loop->for_body = for_body;
    node->node.for_loop->else_body = else_body;
    return node;
}

void for_loop_node_free(for_loop_node_t* for_loop_node)
{
    free(for_loop_node->iterable);
    free(for_loop_node->for_body);
    free(for_loop_node->else_body);
    free(for_loop_node);
}

value_t for_loop_node_evaluate(const for_loop_node_t* for_loop_node, context_t* context)
{
    context_t* local_context = context_new(context);

    const value_t iterable = ast_node_evaluate(for_loop_node->iterable, context);

    if (iterable.type != VALUE_TYPE_VECTOR)
    {
        THROW("Iterable should be vector");
    }


    const vector_value_t* iterable_vector = iterable.data.as_vector;

    bool is_stopped = false;

    context_push(local_context, for_loop_node->iterator, value_new_number(NUM_VAL_0), false);

    if (iterable_vector->size > 0)
    {
        for (ull_t i = 0; i < iterable_vector->size; i++)
        {
            context_set(local_context, for_loop_node->iterator, iterable_vector->values[i]);

            value_t result = ast_node_evaluate(for_loop_node->for_body, local_context);

            switch (result.control_flow)
            {
                case CONTROL_FLOW_BREAK:
                    result.control_flow = CONTROL_FLOW_NONE;
                    is_stopped = true;
                    break;
                case CONTROL_FLOW_RETURN:
                case CONTROL_FLOW_PANIC: return result;
                default: break;
            }
        }
    }

    context_free(local_context);

    if (!is_stopped && for_loop_node->else_body)
    {
        local_context = context_new(context);
        const value_t result = ast_node_evaluate(for_loop_node->else_body, local_context);
        context_free(local_context);
        return result;
    }

    return value_new_null();
}
