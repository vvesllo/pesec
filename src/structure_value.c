#include "include/structure_value.h"

#include <stdlib.h>

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