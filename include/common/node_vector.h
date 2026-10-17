#ifndef PESEC_NODE_VECTOR_H
#define PESEC_NODE_VECTOR_H

#include "include/ast/binary_node.h"

typedef struct
{
    pesec_node_t** items;
    size_t capacity;
    size_t size;
} pesec_node_vector_t;

pesec_node_vector_t pesec_node_vector_new();

void pesec_node_vector_push(pesec_node_vector_t* node_vector, pesec_node_t* node);

void pesec_node_vector_destroy(const pesec_node_vector_t* node_vector);

#endif // PESEC_NODE_VECTOR_H
