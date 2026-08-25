#include "include/module_value.h"

#include <stdlib.h>

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