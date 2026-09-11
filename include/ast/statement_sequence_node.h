#ifndef PESEC_STATEMENT_SEQUENCE_NODE_H
#define PESEC_STATEMENT_SEQUENCE_NODE_H

#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct STATEMENT_SEQUENCE_NODE_QUEUE_STRUCT statement_sequence_node_queue_t;

typedef struct STATEMENT_SEQUENCE_NODE_QUEUE_STRUCT
{
    ast_node_t* statement;
    statement_sequence_node_queue_t* next;
} statement_sequence_node_queue_t;

typedef struct STATEMENT_SEQUENCE_NODE_STRUCT
{
    bool local;
    u64_t count;
    statement_sequence_node_queue_t* statements;
} statement_sequence_node_t;

ast_node_t* statement_sequence_node_new(bool local);

void statement_sequence_node_push(statement_sequence_node_t* statement_sequence_node, ast_node_t* statement);

void statement_sequence_node_free(statement_sequence_node_t* statement_sequence_node);

value_t statement_sequence_node_evaluate(const statement_sequence_node_t* statement_sequence_node, context_t* context);

#endif // PESEC_STATEMENT_SEQUENCE_NODE_H
