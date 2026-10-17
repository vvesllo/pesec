#ifndef PESEC_DEFINITION_NODE_H
#define PESEC_DEFINITION_NODE_H

#include "include/common/string_view.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_string_view_t name;
    pesec_node_t* value;
    bool immutable;
} pesec_definition_node_t;

pesec_node_t* pesec_definition_node_new(
    pesec_string_view_t name,
    pesec_node_t* value,
    bool immutable
);

pesec_value_t pesec_definition_node_evaluate(
    const pesec_definition_node_t* definition_node,
    pesec_environment_t* environment
);

void pesec_definition_node_destroy(const pesec_definition_node_t* definition_node);

#endif // PESEC_DEFINITION_NODE_H
