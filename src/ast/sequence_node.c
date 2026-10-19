#include "include/ast/node.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_sequence_node_new(const pesec_node_vector_t node_vector)
{
    const auto sequence_node = (pesec_sequence_node_t){
        .node_vector = node_vector,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_SEQUENCE,
        (pesec_node_data_t){
            .sequence_node = sequence_node
        }
    );
}

pesec_value_t pesec_sequence_node_evaluate(
    const pesec_sequence_node_t* sequence_node,
    pesec_environment_t* environment
)
{
    const pesec_node_vector_t* node_vector = &sequence_node->node_vector;
    pesec_environment_t* local_environment = pesec_environment_new(environment);
    pesec_value_t value = pesec_value_i64(0);

    for (size_t i=0; i < node_vector->size; i++)
    {
        value = pesec_node_evaluate(node_vector->items[i], local_environment);
        if (value.control_flow != PESEC_VALUE_CONTROL_FLOW_NONE)
            break;
    }

    pesec_environment_destroy(local_environment);

    return value;
}

void pesec_sequence_node_destroy(const pesec_sequence_node_t* sequence_node)
{
    pesec_node_vector_destroy(&sequence_node->node_vector);
}
