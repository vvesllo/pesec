#include "include/ast/node.h"

#include "include/common/alloc.h"

pesec_node_t* pesec_node_new(const pesec_node_type_t type, const pesec_node_data_t data)
{
    const auto node = (pesec_node_t*)pesec_malloc(sizeof(pesec_node_t));
    node->type = type;
    node->data = data;
    return node;
}

int64_t pesec_node_evaluate(const pesec_node_t* node)
{
    switch (node->type)
    {
        case PESEC_NODE_TYPE_LITERAL: return pesec_literal_node_evaluate(&node->data.literal_node);
        case PESEC_NODE_TYPE_BINARY: return pesec_binary_node_evaluate(&node->data.binary_node);
    }

    return 0;
}

void pesec_node_destroy(const pesec_node_t* node)
{
    switch (node->type)
    {
        case PESEC_NODE_TYPE_LITERAL: pesec_literal_node_destroy(&node->data.literal_node); break;
        case PESEC_NODE_TYPE_BINARY: pesec_binary_node_destroy(&node->data.binary_node); break;
    }
}
