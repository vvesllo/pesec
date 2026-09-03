#include "include/ast/vector_access_node.h"

#include "include/ast/ast_node.h"
#include <stdlib.h>

#include "include/vector_value.h"
#include "include/utils/throw.h"


ast_node_t* vector_access_node_new(ast_node_t* vector, ast_node_t* index)
{
    const auto node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_VECTOR_ACCESS;
    node->node.vector_access = (vector_access_node_t*)malloc(sizeof(vector_access_node_t));
    node->node.vector_access->vector = vector;
    node->node.vector_access->index = index;
    return node;
}

void vector_access_node_free(vector_access_node_t* vector_access_node)
{
    ast_node_free(vector_access_node->vector);
    ast_node_free(vector_access_node->index);
    free(vector_access_node);
}

value_t vector_access_node_evaluate(const vector_access_node_t* vector_access_node, context_t* context)
{
    const value_t vector = ast_node_evaluate(vector_access_node->vector, context);

    if (vector.type != VALUE_TYPE_VECTOR)
        THROW("Variable is not vector");

    const value_t index = ast_node_evaluate(vector_access_node->index, context);
    if (index.type != VALUE_TYPE_NUMBER)
        THROW("Index should be an number");

    const vector_value_t* vector_value = vector.data.as_vector;


    return vector_value_get(vector_value, (long long)number_value_to_long_double(index.data.as_number));
}
