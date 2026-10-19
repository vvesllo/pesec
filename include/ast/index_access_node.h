#ifndef PESEC_INDEX_ACCESS_NODE_H
#define PESEC_INDEX_ACCESS_NODE_H

#include "include/common/node_vector.h"
#include "include/runtime/value.h"

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    pesec_node_t* target;
    pesec_node_t* index;
} pesec_index_access_node_t;

pesec_node_t* pesec_index_access_node_new(pesec_node_t* target, pesec_node_t* index);

pesec_value_t pesec_index_access_node_evaluate(const pesec_index_access_node_t* index_access_node, pesec_environment_t* environment);

void pesec_index_access_node_destroy(const pesec_index_access_node_t* index_access_node);

#endif // PESEC_INDEX_ACCESS_NODE_H
