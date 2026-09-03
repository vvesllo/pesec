#ifndef PESEC_AST_NODE_H
#define PESEC_AST_NODE_H

#include "binary_op_node.h"
#include "literal_node.h"
#include "variable_node.h"
#include "variable_definition_node.h"
#include "variable_assignment_node.h"
#include "function_call_node.h"
#include "function_definition_node.h"
#include "structure_definition_node.h"
#include "variable_field_access_node.h"
#include "vector_definition_node.h"
#include "vector_access_node.h"
#include "statement_sequence_node.h"
#include "condition_node.h"
#include "while_loop_node.h"
#include "break_node.h"
#include "for_loop_node.h"
#include "import_node.h"
#include "panic_node.h"
#include "unary_op_node.h"
#include "return_node.h"

#include "include/context.h"

typedef enum
{
    AST_NODE_LITERAL,
    AST_NODE_UNARY,
    AST_NODE_BINARY,
    AST_NODE_VARIABLE,
    AST_NODE_VARIABLE_DEFINITION,
    AST_NODE_VARIABLE_ASSIGNMENT,
    AST_NODE_FUNCTION_CALL,
    AST_NODE_FUNCTION_DEFINITION,
    AST_NODE_STRUCTURE_DEFINITION,
    AST_NODE_VARIABLE_FIELD_ACCESS,
    AST_NODE_VECTOR_DEFINITION,
    AST_NODE_VECTOR_ACCESS,
    AST_NODE_STATEMENT_SEQUENCE,
    AST_NODE_CONDITION,
    AST_NODE_WHILE_LOOP,
    AST_NODE_FOR_LOOP,
    AST_NODE_BREAK,
    AST_NODE_IMPORT,
    AST_NODE_PANIC,
    AST_NODE_RETURN,
} ast_node_type_t;

typedef struct AST_NODE_STRUCT
{
    ast_node_type_t type;
    union
    {
        literal_node_t* literal;
        binary_op_node_t* binary_op;
        unary_op_node_t* unary_op;
        variable_node_t* variable;
        variable_definition_node_t* variable_definition;
        variable_assignment_node_t* variable_assignment;
        function_call_node_t* function_call;
        function_definition_node_t* function_definition;
        structure_definition_node_t* structure_definition;
        variable_field_access_node_t* variable_field_access;
        vector_definition_node_t* vector_definition;
        vector_access_node_t* vector_access;
        statement_sequence_node_t* statement_sequence;
        condition_node_t* condition;
        while_loop_node_t* while_loop;
        for_loop_node_t* for_loop;
        panic_node_t* panic;
        break_node_t* _break;
        import_node_t* _import;
        return_node_t* _return;
    } node;
} ast_node_t;

value_t ast_node_evaluate(const ast_node_t* node, context_t* context);

void ast_node_free(ast_node_t* node);

#endif // PESEC_AST_NODE_H