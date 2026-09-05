#include "include/module_value.h"

#include <stdlib.h>

#include "include/vector_value.h"
#include "include/ast/ast_node.h"

module_value_t* module_value_new(context_t* context)
{
    const auto module_value = (module_value_t*) malloc(sizeof(module_value_t));

    module_value->context = context;

    return module_value;
}

void module_value_set(const module_value_t* module_value, const string_view_t name, const value_t value)
{
    context_set(module_value->context, name, value);
}

value_t module_value_get(const module_value_t* module_value, const string_view_t name)
{
    return context_get_local(module_value->context, name)->value;
}

void module_value_free(module_value_t* module_value)
{
    context_free(module_value->context);
    free(module_value);
}

value_t module_value_get_fields(const module_value_t* module_value)
{
    const context_keys_t* keys = module_value->context->keys;

    vector_value_t* fields_vector = vector_value_new_size(0);
    for (ull_t i = 0; i < keys->size; i++)
        vector_value_push(fields_vector, value_new_string(string_value_from_string_view(keys->keys[i])));

    return value_new_vector(fields_vector);
}