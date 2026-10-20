#include "include/ast/node.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/object_value.h"
#include "include/runtime/vector_value.h"

pesec_node_t* pesec_assignment_node_new(pesec_node_t* target, pesec_node_t* node_value)
{
    const auto assignment_node = (pesec_assignment_node_t){
        .target = target,
        .node_value = node_value,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_ASSIGNMENT,
        (pesec_node_data_t){
            .assignment_node = assignment_node
        }
    );
}

pesec_value_t pesec_assignment_node_evaluate(
    const pesec_assignment_node_t* assignment_node,
    pesec_environment_t* environment
)
{
    const pesec_value_t value = pesec_node_evaluate(assignment_node->node_value, environment);

    switch (assignment_node->target->type)
    {
        case PESEC_NODE_TYPE_VARIABLE:
            pesec_environment_set_variable(
                environment,
                assignment_node->target->data.variable_node.name,
                value
            );
            break;
        case PESEC_NODE_TYPE_FIELD_ACCESS:
            const pesec_value_t object_value = pesec_node_evaluate(
                assignment_node->target->data.field_access_node.object,
                environment
            );

            if (object_value.type != PESEC_VALUE_TYPE_OBJECT)
                pesec_panic("assignment_node::evaluate: cannot assign a field to a non object");

            pesec_object_value_set(
                object_value.data.object,
                assignment_node->target->data.field_access_node.field,
                value
            );

            break;
        case PESEC_NODE_TYPE_INDEX_ACCESS:
            const pesec_value_t vector_value = pesec_node_evaluate(
                assignment_node->target->data.index_access_node.target,
                environment
            );

            if (vector_value.type != PESEC_VALUE_TYPE_VECTOR)
                pesec_panic("assignment_node::evaluate: cannot assign a index to a non vector");

            const pesec_value_t index_value = pesec_node_evaluate(
                assignment_node->target->data.index_access_node.index,
                environment
            );

            if (index_value.type != PESEC_VALUE_TYPE_I64)
                pesec_panic("assignment_node::evaluate: index should be an i64");

            pesec_vector_value_set(
                vector_value.data.vector,
                index_value.data.i64,
                value
            );
            break;
        default:
            pesec_panic("assignment_node::evaluate: invalid assignment target");
    }

    return value;
}

void pesec_assignment_node_destroy(const pesec_assignment_node_t* assignment_node)
{
    pesec_node_destroy(assignment_node->node_value);
}
