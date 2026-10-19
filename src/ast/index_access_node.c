#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/vector_value.h"

pesec_node_t* pesec_index_access_node_new(pesec_node_t* target, pesec_node_t* index)
{
    const auto index_access_node = (pesec_index_access_node_t){
        .target = target,
        .index = index,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_INDEX_ACCESS,
        (pesec_node_data_t){
            .index_access_node = index_access_node
        }
    );
}

pesec_value_t pesec_index_access_node_evaluate(const pesec_index_access_node_t* index_access_node, pesec_environment_t* environment)
{
    const pesec_value_t vector_value = pesec_node_evaluate(index_access_node->target, environment);
    const pesec_value_t index_value = pesec_node_evaluate(index_access_node->index, environment);

    if (vector_value.type != PESEC_VALUE_TYPE_VECTOR)
    {
        pesec_panic("index_access_node::evaluate: can't get index from non vector value");
    }
    if (index_value.type != PESEC_VALUE_TYPE_I64)
    {
        pesec_panic("index_access_node::evaluate: index should be an i64");
    }

    const pesec_vector_value_t* vector = vector_value.data.vector;
    const int64_t index = index_value.data.i64;

    if (index > vector->size && index < -vector->size)
    {
        pesec_panic("index_access_node::evaluate: index out of range");
    }

    return vector->items[index < 0 ? vector->size + index : index];
}

void pesec_index_access_node_destroy(const pesec_index_access_node_t* index_access_node)
{
    pesec_node_destroy(index_access_node->target);
    pesec_node_destroy(index_access_node->index);
}
