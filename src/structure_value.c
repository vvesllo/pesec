#include "include/structure_value.h"

#include <stdlib.h>

#include "include/vector_value.h"
#include "include/ast/ast_node.h"

structure_value_t* structure_value_new(context_t* context)
{
    const auto structure_value = (structure_value_t*) malloc(sizeof(structure_value_t));

    structure_value->context = context;

    return structure_value;
}

void structure_value_set(const structure_value_t* structure_value, const string_view_t name, const value_t value)
{
    context_set(structure_value->context, name, value);
}

value_t structure_value_get(const structure_value_t* structure_value, const string_view_t name)
{
    return context_get_local(structure_value->context, name)->value;
}

void structure_value_free(structure_value_t* structure_value)
{
    context_free(structure_value->context);
    free(structure_value);
}

value_t structure_value_get_fields(const structure_value_t* structure_value)
{
    const context_keys_t* keys = structure_value->context->keys;

    vector_value_t* fields_vector = vector_value_new_size(0);
    for (ull_t i = 0; i < keys->size; i++)
        vector_value_push(fields_vector, value_new_string(string_value_from_string_view(keys->keys[i])));

    return value_new_vector(fields_vector);
}
