#include "include/ast/value_meta_op_node.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/ast/ast_node.h"
#include "include/utils/throw.h"

ast_node_t* value_meta_op_node_new(ast_node_t* value, string_view_t _operator)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VALUE_META_OP;
    node->node.value_meta_op = (value_meta_op_node_t*)malloc(sizeof(value_meta_op_node_t));
    node->node.value_meta_op->value = value;
    node->node.value_meta_op->_operator = _operator;
    return node;
}

void value_meta_op_node_free(value_meta_op_node_t* value_meta_op_node)
{
    ast_node_free(value_meta_op_node->value);
    free(value_meta_op_node);
}

value_t value_meta_op_node_evaluate(const value_meta_op_node_t* value_meta_op_node, context_t* context)
{
    const value_t value = ast_node_evaluate(value_meta_op_node->value, context);
    switch(value.type)
    {
        case VALUE_TYPE_BOOLEAN:
    }
}
