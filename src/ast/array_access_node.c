#include "include/ast/array_access_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/array_value.h"
#include "include/utils/throw.h"


ast_node_t* array_access_node_new(ast_node_t* array, ast_node_t* index)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_ARRAY_ACCESS;
    node->node.array_access = (array_access_node_t*)malloc(sizeof(array_access_node_t));
    node->node.array_access->array = array;
    node->node.array_access->index = index;
    return node;
}

void array_access_node_free(array_access_node_t* array_access_node)
{
    ast_node_free(array_access_node->array);
    ast_node_free(array_access_node->index);
    free(array_access_node);
}

value_t array_access_node_evaluate(const array_access_node_t* array_access_node, context_t* context)
{
    const value_t array = ast_node_evaluate(array_access_node->array, context);

    if (array.type != VALUE_TYPE_ARRAY)
        THROW("Variable is not array");

    const value_t index = ast_node_evaluate(array_access_node->index, context);
    if (index.type != VALUE_TYPE_NUMBER)
        THROW("Index should be an number");

    const array_value_t* array_value = array.data.as_array;


    return array_value_get(array_value, (long long)number_value_to_long_double(index.data.as_number));
}
