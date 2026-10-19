#ifndef PESEC_RANGE_NODE_H
#define PESEC_RANGE_NODE_H

#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* begin;
    pesec_node_t* end;
    bool contains;
} pesec_range_node_t;

pesec_node_t* pesec_range_node_new(pesec_node_t* begin, pesec_node_t*, bool contains);

pesec_value_t pesec_range_node_evaluate(const pesec_range_node_t* range_node, pesec_environment_t* environment);

void pesec_range_node_destroy(const pesec_range_node_t* range_node);

#endif // PESEC_RANGE_NODE_H
