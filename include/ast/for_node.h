#ifndef PESEC_FOR_NODE_H
#define PESEC_FOR_NODE_H

#include "include/common/parameter_vector.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_string_view_t iterator;
    pesec_node_t* iterable;
    pesec_node_t* body;
} pesec_for_node_t;

pesec_node_t* pesec_for_node_new(pesec_string_view_t iterator, pesec_node_t* iterable, pesec_node_t* body);

pesec_value_t pesec_for_node_evaluate(const pesec_for_node_t* for_node, pesec_environment_t* environment);

void pesec_for_node_destroy(const pesec_for_node_t* for_node);

#endif // PESEC_FOR_NODE_H
