#ifndef PESEC_NODE_H
#define PESEC_NODE_H

#include <inttypes.h>

#include "literal_node.h"
#include "binary_node.h"

typedef enum
{
    PESEC_NODE_TYPE_LITERAL,
    PESEC_NODE_TYPE_BINARY,
} pesec_node_type_t;

typedef union
{
    pesec_literal_node_t literal_node;
    pesec_binary_node_t binary_node;
} pesec_node_data_t;

typedef struct PESEC_NODE_STRUCT
{
    pesec_node_type_t type;
    pesec_node_data_t data;
} pesec_node_t;

pesec_node_t* pesec_node_new(pesec_node_type_t type, pesec_node_data_t data);

int64_t pesec_node_evaluate(const pesec_node_t* node);

void pesec_node_destroy(const pesec_node_t* node);

#endif // PESEC_NODE_H