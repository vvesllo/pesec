#ifndef PESEC_VALUE_META_OP_NODE_H
#define PESEC_VALUE_META_OP_NODE_H

#include "include/token.h"
#include "include/value.h"
#include "include/context.h"

typedef struct AST_NODE_STRUCT ast_node_t;

typedef struct
{
    ast_node_t* value;
    string_view_t _operator;
} value_meta_op_node_t;

ast_node_t* value_meta_op_node_new(ast_node_t* value, string_view_t _operator);

void value_meta_op_node_free(value_meta_op_node_t* value_meta_op_node);

value_t value_meta_op_node_evaluate(const value_meta_op_node_t* value_meta_op_node, context_t* context);

#endif // PESEC_VALUE_META_OP_NODE_H
