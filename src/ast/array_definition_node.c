#include "include/ast/array_definition_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/array_value.h"


ast_node_t* array_definition_node_new(ast_node_t* values)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_ARRAY_DEFINITION;
    node->node.array_definition = (array_definition_node_t*)malloc(sizeof(array_definition_node_t));
    node->node.array_definition->values = values;
    return node;
}

void array_definition_node_free(array_definition_node_t* array_definition_node)
{
    free(array_definition_node);
}

value_t array_definition_node_evaluate(const array_definition_node_t* array_definition_node, context_t* context)
{
    const ull_t count = array_definition_node->values->node.statement_sequence->count;
    const auto values = (value_t*)calloc(count, sizeof(value_t));

    const statement_sequence_node_queue_t* current = array_definition_node->values->node.statement_sequence->statements;

    for (ull_t i = 0; i < count; ++i)
    {
        values[i] = ast_node_evaluate(current->statement, context);
        current = current->next;
    }

    return value_new_array(array_value_new(values, count));
}
