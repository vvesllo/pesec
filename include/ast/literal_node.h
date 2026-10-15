#ifndef PESEC_LITERAL_NODE_H
#define PESEC_LITERAL_NODE_H

#include <inttypes.h>

typedef struct PESEC_NODE_STRUCT pesec_node_t;

typedef struct
{
    int64_t value;
} pesec_literal_node_t;

pesec_node_t* pesec_literal_node_new(int64_t value);

int64_t pesec_literal_node_evaluate(const pesec_literal_node_t* literal_node);

void pesec_literal_node_destroy(const pesec_literal_node_t* literal_node);

#endif // PESEC_LITERAL_NODE_H