#include "include/ast/node.h"

pesec_node_t* pesec_literal_node_new(const pesec_value_t value)
{
    const auto literal_node = (pesec_literal_node_t){.value = value};
    return pesec_node_new(
        PESEC_NODE_TYPE_LITERAL,
        (pesec_node_data_t){
            .literal_node = literal_node
        }
    );
}

pesec_value_t pesec_literal_node_evaluate(const pesec_literal_node_t* literal_node, pesec_environment_t* environment)
{
    return literal_node->value;
}

void pesec_literal_node_destroy(const pesec_literal_node_t* literal_node)
{
}
