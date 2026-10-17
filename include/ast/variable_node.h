#ifndef PESEC_VARIABLE_NODE_H
#define PESEC_VARIABLE_NODE_H

#include "include/common/string_view.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_string_view_t name;
} pesec_variable_node_t;

pesec_node_t* pesec_variable_node_new(pesec_string_view_t name);

pesec_value_t pesec_variable_node_evaluate(const pesec_variable_node_t* variable_node, const pesec_environment_t* environment);

void pesec_variable_node_destroy(const pesec_variable_node_t* variable_node);

#endif // PESEC_VARIABLE_NODE_H