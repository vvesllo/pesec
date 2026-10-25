#include "include/common/node_vector.h"

#include "include/ast/node.h"
#include "include/common/alloc.h"

pesec_node_vector_t pesec_node_vector_new()
{
    pesec_node_vector_t node_vector;

    node_vector.size = 0;
    node_vector.capacity = 64;
    node_vector.items = (pesec_node_t**)pesec_calloc(node_vector.capacity, sizeof(pesec_node_t*));

    return node_vector;
}

void pesec_node_vector_push(pesec_node_vector_t* node_vector, pesec_node_t* node)
{
    if (node_vector->size >= node_vector->capacity)
    {
        node_vector->capacity <<= 1;
        node_vector->items = (pesec_node_t**)pesec_realloc(node_vector->items, node_vector->capacity * sizeof(pesec_node_t*));
    }

    node_vector->items[node_vector->size] = node;
    node_vector->size += 1;
}

void pesec_node_vector_destroy(const pesec_node_vector_t* node_vector)
{
    for (size_t i =0; i < node_vector->size; i++)
    {
        pesec_node_destroy(node_vector->items[i]);
    }
    pesec_free(node_vector->items);
}
