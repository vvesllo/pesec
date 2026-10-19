#ifndef PESEC_OBJECT_NODE_H
#define PESEC_OBJECT_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_vector_t node_vector;
} pesec_object_node_t;

pesec_node_t* pesec_object_node_new(pesec_node_vector_t node_vector);

pesec_value_t pesec_object_node_evaluate(const pesec_object_node_t* object_node, pesec_environment_t* environment);

void pesec_object_node_destroy(const pesec_object_node_t* object_node);

#endif // PESEC_OBJECT_NODE_H