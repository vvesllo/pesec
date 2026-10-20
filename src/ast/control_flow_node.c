#include "include/ast/node.h"

pesec_node_t* pesec_control_flow_node_new(pesec_node_t* value, const pesec_value_control_flow_t control_flow)
{
    const auto control_flow_node = (pesec_control_flow_node_t){
        .value = value,
        .control_flow = control_flow,
    };
    return pesec_node_new(
        PESEC_NODE_TYPE_CONTROL_FLOW,
        (pesec_node_data_t){
            .control_flow_node = control_flow_node
        }
    );
}

pesec_value_t pesec_control_flow_node_evaluate(const pesec_control_flow_node_t* control_flow_node, pesec_environment_t* environment)
{
    pesec_value_t value = pesec_value_as_none();
    if (control_flow_node->value)
    {
        value = pesec_node_evaluate(control_flow_node->value, environment);
    }

    value.control_flow = control_flow_node->control_flow;
    return value;
}

void pesec_control_flow_node_destroy(const pesec_control_flow_node_t* control_flow_node)
{
    pesec_node_destroy(control_flow_node->value);
}
