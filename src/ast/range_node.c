#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_range_node_new(pesec_node_t* begin, pesec_node_t* end, const bool contains)
{
    const auto range_node = (pesec_range_node_t){
        .begin = begin,
        .end = end,
        .contains = contains,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_RANGE,
        (pesec_node_data_t){
            .range_node = range_node
        }
    );
}

pesec_value_t pesec_range_node_evaluate(const pesec_range_node_t* range_node, pesec_environment_t* environment)
{
    const pesec_value_t begin_value = pesec_node_evaluate(range_node->begin, environment);
    const pesec_value_t end_value = pesec_node_evaluate(range_node->end, environment);

    if (begin_value.type != PESEC_VALUE_TYPE_I64)
        pesec_panic("range_node::evaluate: begin value should be i64");

    if (end_value.type != PESEC_VALUE_TYPE_I64)
        pesec_panic("range_node::evaluate: end value should be i64");

    pesec_vector_value_t vector_value = pesec_vector_value_new();

    const int64_t begin_i64 = begin_value.data.i64;
    const int64_t end_i64 = end_value.data.i64;

    if (begin_i64 < end_i64)
    {
        for (int64_t i = begin_i64; i < end_i64 + range_node->contains; i++)
        {
            pesec_vector_value_push(&vector_value, pesec_value_i64(i));
        }
    }
    else
    {
        for (int64_t i = begin_i64; i > end_i64 - range_node->contains; i--)
        {
            pesec_vector_value_push(&vector_value, pesec_value_i64(i));
        }
    }

    return pesec_value_vector(vector_value);
}

void pesec_range_node_destroy(const pesec_range_node_t* range_node)
{
    pesec_node_destroy(range_node->begin);
    pesec_node_destroy(range_node->end);
}
