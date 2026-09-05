#include "include/ast/value_meta_op_node.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/module_value.h"
#include "include/structure_value.h"
#include "include/vector_value.h"
#include "include/ast/ast_node.h"
#include "include/utils/throw.h"

ast_node_t* value_meta_op_node_new(ast_node_t* value, const string_view_t _operator)
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

static value_t meta_value_meta_op_node_fields(const value_t value)
{
    switch(value.type)
    {
        case VALUE_TYPE_BOOLEAN:
        case VALUE_TYPE_NULL:
        case VALUE_TYPE_NUMBER:
        case VALUE_TYPE_FUNCTION:
            return value_new_vector(vector_value_new_size(0));
        case VALUE_TYPE_STRING: return string_value_get_fields(value.data.as_string);
        case VALUE_TYPE_VECTOR: return vector_value_get_fields(value.data.as_vector);
        case VALUE_TYPE_STRUCTURE: return structure_value_get_fields(value.data.as_structure);
        case VALUE_TYPE_MODULE: return module_value_get_fields(value.data.as_module);
    }

    THROW("Unknown type");
}

static value_t meta_value_meta_op_node_type(const value_t value)
{
    return value_new_string(string_value_from_cstr(value_get_type(value)));
}

value_t value_meta_op_node_evaluate(const value_meta_op_node_t* value_meta_op_node, context_t* context)
{
    const value_t value = ast_node_evaluate(value_meta_op_node->value, context);

    if (string_view_equals_cstr(value_meta_op_node->_operator, "fields")) return meta_value_meta_op_node_fields(value);
    if (string_view_equals_cstr(value_meta_op_node->_operator, "type")) return meta_value_meta_op_node_type(value);

    THROW("Unknown meta operator '%.*s'", (int)value_meta_op_node->_operator.length, value_meta_op_node->_operator.data);
}
