#include "include/ast/node.h"
#include "include/runtime/environment.h"

pesec_node_t* pesec_assignment_node_new(const pesec_string_view_t name, pesec_node_t* node_value)
{
    const auto assignment_node = (pesec_assignment_node_t){
        .name = name,
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

    pesec_environment_set_variable(
        environment,
        assignment_node->name,
        value
    );

    return value;
}

void pesec_assignment_node_destroy(const pesec_assignment_node_t* assignment_node)
{
    // nothing to do
}
