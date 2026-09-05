#include "include/ast/variable_assignment_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/vector_value.h"
#include "include/structure_value.h"
#include "include/utils/throw.h"


ast_node_t* variable_assignment_node_new(ast_node_t* target, ast_node_t* value)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VARIABLE_ASSIGNMENT;
    node->node.variable_assignment = (variable_assignment_node_t*)malloc(sizeof(variable_assignment_node_t));
    node->node.variable_assignment->target = target;
    node->node.variable_assignment->value = value;
    return node;
}

void variable_assignment_node_free(variable_assignment_node_t* variable_assignment_node)
{
    if (!variable_assignment_node) return;
    if (variable_assignment_node->target) ast_node_free(variable_assignment_node->target);
    if (variable_assignment_node->value) ast_node_free(variable_assignment_node->value);
    free(variable_assignment_node);
}

value_t variable_assignment_node_evaluate(const variable_assignment_node_t* variable_assignment_node, context_t* context)
{
    const value_t value = ast_node_evaluate(variable_assignment_node->value, context);

    switch (variable_assignment_node->target->type)
    {
        case AST_NODE_VARIABLE:
            context_set(context, variable_assignment_node->target->node.variable->name, value);
            break;
        case AST_NODE_VARIABLE_FIELD_ACCESS:
            const value_t object_value = ast_node_evaluate(
                variable_assignment_node->target->node.variable_field_access->object,
                context
            );

            if (object_value.type != VALUE_TYPE_STRUCTURE)
                THROW("Cannot assign a field to a non structure\n");

            structure_value_set(
                object_value.data.as_structure,
                variable_assignment_node->target->node.variable_field_access->field,
                value
            );

            break;
        case AST_NODE_VECTOR_ACCESS:
            const value_t vector_value = ast_node_evaluate(
                variable_assignment_node->target->node.vector_access->vector,
                context
            );

            if (vector_value.type != VALUE_TYPE_VECTOR)
                THROW("Cannot assign a index to a non vector\n");

            const value_t index_value = ast_node_evaluate(
                variable_assignment_node->target->node.vector_access->index,
                context
            );

            if (index_value.type != VALUE_TYPE_NUMBER)
                THROW("Index should be a number\n");

            vector_value_set(
                vector_value.data.as_vector,
                (long long)number_value_to_long_double(index_value.data.as_number),
                value
            );

            break;
        default:
            THROW("Invalid assignment target\n");
    }

    return value;
}
