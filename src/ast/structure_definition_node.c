#include "include/ast/structure_definition_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/structure_value.h"


ast_node_t* structure_definition_node_new(ast_node_t* fields)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_STRUCTURE_DEFINITION;
    node->node.structure_definition = (structure_definition_node_t*)malloc(sizeof(structure_definition_node_t));
    node->node.structure_definition->fields = fields;
    return node;
}

void structure_definition_node_free(structure_definition_node_t* structure_definition_node)
{
    ast_node_free(structure_definition_node->fields);
    free(structure_definition_node);
}

value_t structure_definition_node_evaluate(const structure_definition_node_t* structure_definition_node, context_t* context)
{
    const statement_sequence_node_t* fields_sequence = structure_definition_node->fields->node.statement_sequence;
    const statement_sequence_node_queue_t* current_field = fields_sequence->statements;

    context_t* structure_value_context = context_new(context);

    while (current_field)
    {
        ast_node_evaluate(current_field->statement, structure_value_context);
        current_field = current_field->next;
    }

    return MAKE_VAL_STRUCT(structure_value_new(structure_value_context));
}
