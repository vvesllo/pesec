#ifndef PESEC_SEQUENCE_NODE_H
#define PESEC_SEQUENCE_NODE_H

#include "include/common/node_vector.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_vector_t node_vector;
} pesec_sequence_node_t;

pesec_node_t* pesec_sequence_node_new(pesec_node_vector_t node_vector);

pesec_value_t pesec_sequence_node_evaluate(const pesec_sequence_node_t* sequence_node, pesec_environment_t* environment);

void pesec_sequence_node_destroy(const pesec_sequence_node_t* sequence_node);

#endif // PESEC_SEQUENCE_NODE_H
