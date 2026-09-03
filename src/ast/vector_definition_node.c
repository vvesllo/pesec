#include "include/ast/vector_definition_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/vector_value.h"


ast_node_t* vector_definition_node_new(ast_node_t* values)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VECTOR_DEFINITION;
    node->node.vector_definition = (vector_definition_node_t*)malloc(sizeof(vector_definition_node_t));
    node->node.vector_definition->values = values;
    return node;
}

void vector_definition_node_free(vector_definition_node_t* vector_definition_node)
{
    ast_node_free(vector_definition_node->values);
    free(vector_definition_node);
}

value_t vector_definition_node_evaluate(const vector_definition_node_t* vector_definition_node, context_t* context)
{
    const ull_t count = vector_definition_node->values->node.statement_sequence->count;
    const auto values = (value_t*)calloc(count, sizeof(value_t));

    const statement_sequence_node_queue_t* current = vector_definition_node->values->node.statement_sequence->statements;

    for (ull_t i = 0; i < count; ++i)
    {
        values[i] = ast_node_evaluate(current->statement, context);
        current = current->next;
    }

    return value_new_vector(vector_value_new(values, count));
}
