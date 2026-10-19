#ifndef PESEC_CONTROL_FLOW_NODE_H
#define PESEC_CONTROL_FLOW_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* value;
    pesec_value_control_flow_t control_flow;
} pesec_control_flow_node_t;

pesec_node_t* pesec_control_flow_node_new(pesec_node_t* value, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_control_flow_node_evaluate(const pesec_control_flow_node_t* control_flow_node, pesec_environment_t* environment);

void pesec_control_flow_node_destroy(const pesec_control_flow_node_t* control_flow_node);

#endif // PESEC_CONTROL_FLOW_NODE_H