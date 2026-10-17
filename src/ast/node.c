#include "include/ast/node.h"

#include "include/common/alloc.h"

pesec_node_t* pesec_node_new(const pesec_node_type_t type, const pesec_node_data_t data)
{
    const auto node = (pesec_node_t*)pesec_malloc(sizeof(pesec_node_t));
    node->type = type;
    node->data = data;
    return node;
}

pesec_value_t pesec_node_evaluate(const pesec_node_t* node, pesec_environment_t* environment)
{
    switch (node->type)
    {
        case PESEC_NODE_TYPE_LITERAL: return pesec_literal_node_evaluate(&node->data.literal_node, environment);
        case PESEC_NODE_TYPE_BINARY: return pesec_binary_node_evaluate(&node->data.binary_node, environment);
        case PESEC_NODE_TYPE_VARIABLE: return pesec_variable_node_evaluate(&node->data.variable_node, environment);
        case PESEC_NODE_TYPE_FUNCTION_CALL: return pesec_function_call_node_evaluate(&node->data.function_call_node, environment);
    }

    return pesec_value_i64(0);
}

void pesec_node_destroy(const pesec_node_t* node)
{
    switch (node->type)
    {
        case PESEC_NODE_TYPE_LITERAL: pesec_literal_node_destroy(&node->data.literal_node); break;
        case PESEC_NODE_TYPE_BINARY: pesec_binary_node_destroy(&node->data.binary_node); break;
        case PESEC_NODE_TYPE_VARIABLE: pesec_variable_node_destroy(&node->data.variable_node); break;
        case PESEC_NODE_TYPE_FUNCTION_CALL: pesec_function_call_node_destroy(&node->data.function_call_node); break;
    }
}
