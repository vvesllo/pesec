#ifndef PESEC_FIELD_ACCESS_NODE_H
#define PESEC_FIELD_ACCESS_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* object;
    pesec_string_view_t field;
} pesec_field_access_node_t;

pesec_node_t* pesec_field_access_node_new(pesec_node_t* object, pesec_string_view_t field);

pesec_value_t pesec_field_access_node_evaluate(
    const pesec_field_access_node_t* field_access_node,
    pesec_environment_t* environment
);

pesec_value_t pesec_field_access_node_get_field(
    pesec_value_t object_value,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

void pesec_field_access_node_destroy(const pesec_field_access_node_t* field_access_node);

#endif // PESEC_FIELD_ACCESS_NODE_H
