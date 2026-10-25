#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/number_value.h"
#include "include/runtime/vector_value.h"

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

    if (begin_value.type != PESEC_VALUE_TYPE_NUMBER)
        pesec_panic("range_node::evaluate: begin value should be integer");

    if (end_value.type != PESEC_VALUE_TYPE_NUMBER)
        pesec_panic("range_node::evaluate: end value should be integer");

    pesec_vector_value_t* vector_value = pesec_vector_value_new();

    double begin = pesec_number_value_get(begin_value.data.number);
    const double end = pesec_number_value_get(end_value.data.number);

    if (begin < end)
    {
        while (begin < end + range_node->contains)
        {
            const pesec_value_t value = pesec_value_as_number(pesec_number_value_new(begin));
            pesec_vector_value_push(vector_value, value);
            ++begin;
        }
    }
    else
    {
        while (begin > end - range_node->contains)
        {
            const pesec_value_t value = pesec_value_as_number(pesec_number_value_new(begin));
            pesec_vector_value_push(vector_value, value);
            --begin;
        }
    }

    return pesec_value_as_vector(vector_value);
}

void pesec_range_node_destroy(const pesec_range_node_t* range_node)
{
    pesec_node_destroy(range_node->begin);
    pesec_node_destroy(range_node->end);
}
