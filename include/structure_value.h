#ifndef PESEC_STRUCTURE_VALUE_H
#define PESEC_STRUCTURE_VALUE_H

#include "context.h"

typedef struct AST_NODE_STRUCT ast_node_t;
typedef struct STATEMENT_SEQUENCE_NODE_STRUCT statement_sequence_node_t;

typedef struct STRUCTURE_VALUE_STRUCT
{
    context_t* context;
} structure_value_t;

structure_value_t* structure_value_new(context_t* context);

value_t structure_value_get(const structure_value_t* structure_value, string_view_t name);

void structure_value_set(const structure_value_t* structure_value, string_view_t name, value_t value);

void structure_value_free(structure_value_t* structure_value);

#endif // PESEC_STRUCTURE_VALUE_H
