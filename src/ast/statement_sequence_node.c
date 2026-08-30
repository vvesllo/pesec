#include "include/ast/statement_sequence_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

ast_node_t *statement_sequence_node_new(const bool local)
{
    const auto node = (ast_node_t *) malloc(sizeof(ast_node_t));

    node->type = AST_NODE_STATEMENT_SEQUENCE;
    node->node.statement_sequence = (statement_sequence_node_t *) malloc(sizeof(statement_sequence_node_t));
    node->node.statement_sequence->statements = nullptr;
    node->node.statement_sequence->count = 0;
    node->node.statement_sequence->local = local;

    return node;
}

void statement_sequence_node_push(statement_sequence_node_t *statement_sequence_node, ast_node_t *statement)
{
    ++statement_sequence_node->count;

    const auto new_node = (statement_sequence_node_queue_t *) malloc(sizeof(statement_sequence_node_queue_t));

    new_node->statement = statement;
    new_node->next = nullptr;

    if (statement_sequence_node->statements == nullptr)
    {
        statement_sequence_node->statements = new_node;
        return;
    }

    statement_sequence_node_queue_t *current = statement_sequence_node->statements;

    while (current->next)
    {
        current = current->next;
    }

    current->next = new_node;
}

void statement_sequence_node_free(statement_sequence_node_t *statement_sequence_node)
{
    statement_sequence_node_queue_t *current = statement_sequence_node->statements;

    while (current)
    {
        statement_sequence_node_queue_t *next = current->next;

        if (current->statement) ast_node_free(current->statement);

        free(current);
        current = next;
    }

    free(statement_sequence_node);
}

value_t statement_sequence_node_evaluate(const statement_sequence_node_t *statement_sequence_node, context_t *context)
{
    value_t result;
    result.reference_count = 1;
    result.type = VALUE_TYPE_NUMBER;
    result.data.as_number = NUM_VAL_0;
    result.control_flow = CONTROL_FLOW_NONE;

    const statement_sequence_node_queue_t *current = statement_sequence_node->statements;

    context_t *local_context = statement_sequence_node->local ? context_new(context) : context;

    while (current)
    {
        if (current->statement)
        {
            result = ast_node_evaluate(current->statement, local_context);

            switch (result.control_flow)
            {
                case CONTROL_FLOW_BREAK:
                case CONTROL_FLOW_CONTINUE:
                case CONTROL_FLOW_THROW:
                    return result;
                default: break;
            }
        }

        current = current->next;
    }

    if (statement_sequence_node->local)
        context_free(local_context);

    return result;
}
