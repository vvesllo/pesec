#ifndef PESEC_VECTOR_NODE_H
#define PESEC_VECTOR_NODE_H

#include "include/common/parameter_vector.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_vector_t node_vector;
} pesec_vector_node_t;

pesec_node_t* pesec_vector_node_new(pesec_node_vector_t node_vector);

pesec_value_t pesec_vector_node_evaluate(const pesec_vector_node_t* vector_node, pesec_environment_t* environment);

void pesec_vector_node_destroy(const pesec_vector_node_t* vector_node);

#endif // PESEC_VECTOR_NODE_H