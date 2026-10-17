#ifndef PESEC_ASSIGNMENT_NODE_H
#define PESEC_ASSIGNMENT_NODE_H

#include "include/common/string_view.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_string_view_t name;
    pesec_node_t* node_value;
} pesec_assignment_node_t;

pesec_node_t* pesec_assignment_node_new(pesec_string_view_t name, pesec_node_t* node_value);

pesec_value_t pesec_assignment_node_evaluate(const pesec_assignment_node_t* assignment_node, pesec_environment_t* environment);

void pesec_assignment_node_destroy(const pesec_assignment_node_t* assignment_node);

#endif // PESEC_ASSIGNMENT_NODE_H
