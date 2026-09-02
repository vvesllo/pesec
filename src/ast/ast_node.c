#include "include/ast/ast_node.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/utils/throw.h"

value_t ast_node_evaluate(const ast_node_t* node, context_t* context)
{
    switch(node->type)
    {
        case AST_NODE_LITERAL: return literal_node_evaluate(node->node.literal, context);
        case AST_NODE_BINARY: return binary_op_node_evaluate(node->node.binary_op, context);
        case AST_NODE_UNARY: return unary_op_node_evaluate(node->node.unary_op, context);
        case AST_NODE_VARIABLE: return variable_node_evaluate(node->node.variable, context);
        case AST_NODE_VARIABLE_DEFINITION: return variable_definition_node_evaluate(node->node.variable_definition, context);
        case AST_NODE_VARIABLE_ASSIGNMENT: return variable_assignment_node_evaluate(node->node.variable_assignment, context);
        case AST_NODE_FUNCTION_CALL: return function_call_node_evaluate(node->node.function_call, context);
        case AST_NODE_FUNCTION_DEFINITION: return function_definition_node_evaluate(node->node.function_definition, context);
        case AST_NODE_STRUCTURE_DEFINITION: return structure_definition_node_evaluate(node->node.structure_definition, context);
        case AST_NODE_VARIABLE_FIELD_ACCESS: return variable_field_access_node_evaluate(node->node.variable_field_access, context);
        case AST_NODE_ARRAY_DEFINITION: return array_definition_node_evaluate(node->node.array_definition, context);
        case AST_NODE_ARRAY_ACCESS: return array_access_node_evaluate(node->node.array_access, context);
        case AST_NODE_STATEMENT_SEQUENCE: return statement_sequence_node_evaluate(node->node.statement_sequence, context);
        case AST_NODE_CONDITION: return condition_node_evaluate(node->node.condition, context);
        case AST_NODE_WHILE_LOOP: return while_loop_node_evaluate(node->node.while_loop, context);
        case AST_NODE_FOR_LOOP: return for_loop_node_evaluate(node->node.for_loop, context);
        case AST_NODE_PANIC: return panic_node_evaluate(node->node.panic, context);
        case AST_NODE_BREAK: return break_node_evaluate(node->node._break, context);
        case AST_NODE_IMPORT: return import_node_evaluate(node->node._import, context);
        case AST_NODE_RETURN: return return_node_evaluate(node->node._return, context);
    }

    THROW("Unknown node type: %d\n", node->type);
}

void ast_node_free(ast_node_t* node)
{
    switch(node->type)
    {
        case AST_NODE_LITERAL: literal_node_free(node->node.literal); break;
        case AST_NODE_BINARY: binary_op_node_free(node->node.binary_op); break;
        case AST_NODE_UNARY: unary_op_node_free(node->node.unary_op); break;
        case AST_NODE_VARIABLE: variable_node_free(node->node.variable); break;
        case AST_NODE_VARIABLE_DEFINITION: variable_definition_node_free(node->node.variable_definition); break;
        case AST_NODE_VARIABLE_ASSIGNMENT: variable_assignment_node_free(node->node.variable_assignment); break;
        case AST_NODE_FUNCTION_CALL: function_call_node_free(node->node.function_call); break;
        case AST_NODE_FUNCTION_DEFINITION: function_definition_node_free(node->node.function_definition); break;
        case AST_NODE_STRUCTURE_DEFINITION: structure_definition_node_free(node->node.structure_definition); break;
        case AST_NODE_VARIABLE_FIELD_ACCESS: variable_field_access_node_free(node->node.variable_field_access); break;
        case AST_NODE_ARRAY_DEFINITION: array_definition_node_free(node->node.array_definition); break;
        case AST_NODE_ARRAY_ACCESS: array_access_node_free(node->node.array_access); break;
        case AST_NODE_STATEMENT_SEQUENCE: statement_sequence_node_free(node->node.statement_sequence); break;
        case AST_NODE_CONDITION: condition_node_free(node->node.condition); break;
        case AST_NODE_WHILE_LOOP: while_loop_node_free(node->node.while_loop); break;
        case AST_NODE_FOR_LOOP: for_loop_node_free(node->node.for_loop); break;
        case AST_NODE_PANIC: panic_node_free(node->node.panic); break;
        case AST_NODE_BREAK: break_node_free(node->node._break); break;
        case AST_NODE_IMPORT: import_node_free(node->node._import); break;
        case AST_NODE_RETURN: return_node_free(node->node._return); break;
    }

    free(node);
}