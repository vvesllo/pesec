#include "include/ast/node.h"
#include "include/runtime/environment.h"
#include "include/runtime/vector_value.h"

pesec_node_t* pesec_vector_node_new(pesec_node_vector_t node_vector)
{
    const auto vector_node = (pesec_vector_node_t){
        .node_vector = node_vector
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_VECTOR,
        (pesec_node_data_t){
            .vector_node = vector_node
        }
    );
}

pesec_value_t pesec_vector_node_evaluate(const pesec_vector_node_t* vector_node, pesec_environment_t* environment)
{
    pesec_vector_value_t* vector_value = pesec_vector_value_new();

    for (size_t i=0; i < vector_node->node_vector.size; i++)
    {
        const pesec_value_t value = pesec_node_evaluate(vector_node->node_vector.items[i], environment);
        pesec_vector_value_push(vector_value, value);
    }

    return pesec_value_as_vector(vector_value);
}

void pesec_vector_node_destroy(const pesec_vector_node_t* vector_node)
{
    pesec_node_vector_destroy(&vector_node->node_vector);
}
