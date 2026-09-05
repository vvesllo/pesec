#include "include/ast/variable_complex_assignment_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/vector_value.h"
#include "include/structure_value.h"
#include "include/utils/throw.h"


ast_node_t* variable_complex_assignment_node_new(const token_t operation, ast_node_t* target, ast_node_t* value)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VARIABLE_COMPLEX_ASSIGNMENT;
    node->node.variable_complex_assignment = (variable_complex_assignment_node_t*)malloc(sizeof(variable_complex_assignment_node_t));
    node->node.variable_complex_assignment->operation = operation;
    node->node.variable_complex_assignment->target = target;
    node->node.variable_complex_assignment->value = value;
    return node;
}

void variable_complex_assignment_node_free(variable_complex_assignment_node_t* variable_complex_assignment_node)
{
    if (!variable_complex_assignment_node) return;
    if (variable_complex_assignment_node->target) ast_node_free(variable_complex_assignment_node->target);
    if (variable_complex_assignment_node->value) ast_node_free(variable_complex_assignment_node->value);
    free(variable_complex_assignment_node);
}

value_t variable_complex_assignment_node_evaluate(const variable_complex_assignment_node_t* variable_complex_assignment_node, context_t* context)
{
    const value_t value = ast_node_evaluate(variable_complex_assignment_node->value, context);

    value_t (*operation)(value_t, value_t) = nullptr;

    switch (variable_complex_assignment_node->operation.type)
    {
        case TOKEN_TYPE_PLUS_EQUALS: operation = value_operation_add; break;
        case TOKEN_TYPE_MINUS_EQUALS: operation = value_operation_sub; break;
        case TOKEN_TYPE_SLASH_EQUALS: operation = value_operation_div; break;
        case TOKEN_TYPE_SLASH_SLASH_EQUALS: operation = value_operation_floor_div; break;
        case TOKEN_TYPE_ASTERISK_EQUALS: operation = value_operation_mul; break;
        case TOKEN_TYPE_ASTERISK_ASTERISK_EQUALS: operation = value_operation_pow; break;
        default: THROW("Unknown operation '%s'", token_get_type(variable_complex_assignment_node->operation));
    }

    const ast_node_t* target = variable_complex_assignment_node->target;

    switch (target->type)
    {
        case AST_NODE_VARIABLE:
            const string_view_t name = target->node.variable->name;
            context_set(context, name, operation(context_get(context, name)->value, value));
            break;
        case AST_NODE_VARIABLE_FIELD_ACCESS:
            const value_t object_value = ast_node_evaluate(
                target->node.variable_field_access->object,
                context
            );

            if (object_value.type != VALUE_TYPE_STRUCTURE)
                THROW("Cannot assign a field to a non structure\n");

            const string_view_t field = target->node.variable_field_access->field;

            const structure_value_t* structure = object_value.data.as_structure;

            structure_value_set(
                structure,
                field,
                operation(structure_value_get(structure, field), value)
            );

            break;
        case AST_NODE_VECTOR_ACCESS:
            const value_t vector_value = ast_node_evaluate(
                target->node.vector_access->vector,
                context
            );

            if (vector_value.type != VALUE_TYPE_VECTOR)
                THROW("Cannot assign a index to a non vector\n");

            const value_t index_value = ast_node_evaluate(
                target->node.vector_access->index,
                context
            );

            if (index_value.type != VALUE_TYPE_NUMBER)
                THROW("Index should be a number\n");

            const long long index = number_value_to_long_double(index_value.data.as_number);

            vector_value_set(
                vector_value.data.as_vector,
                index,
                operation(vector_value_get(vector_value.data.as_vector, index), value)
            );

            break;
        default:
            THROW("Invalid assignment target\n");
    }

    return value;
}
