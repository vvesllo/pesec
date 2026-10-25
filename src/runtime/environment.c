#include "include/runtime/environment.h"

#include "include/common/alloc.h"
#include "include/common/panic.h"

pesec_environment_t* pesec_environment_new(pesec_environment_t* parent)
{
    const auto environment = (pesec_environment_t*)pesec_malloc(sizeof(pesec_environment_t));

    environment->parent = parent;
    environment->variable_table = pesec_variable_table_new();
    environment->reference_count = 1;
    if (parent) pesec_environment_increase_reference(parent);

    return environment;
}

void pesec_environment_increase_reference(pesec_environment_t* environment)
{
    environment->reference_count += 1;
}

void pesec_environment_decrease_reference(pesec_environment_t* environment)
{
    if (!environment) return;

    environment->reference_count -= 1;

    if (environment->reference_count > 0)
        return;

    pesec_variable_table_destroy(environment->variable_table);

    if (environment->parent)
        pesec_environment_decrease_reference(environment->parent);

    pesec_free(environment);
}

pesec_variable_t* pesec_environment_get_variable(const pesec_environment_t* environment, const pesec_string_view_t name)
{
    pesec_variable_t* variable = pesec_variable_table_find(environment->variable_table, name);
    if (!variable && environment->parent)
        variable = pesec_environment_get_variable(environment->parent, name);
    if (variable) return variable;

    pesec_panic("environment::get_variable: variable `%.*s` doesn't exists",
                name.size, name.data
    );
}

void pesec_environment_set_variable(
    const pesec_environment_t* environment,
    const pesec_string_view_t name,
    pesec_value_t value
)
{
    pesec_variable_t* variable = pesec_variable_table_find(environment->variable_table, name);
    if (!variable && environment->parent)
        variable = pesec_environment_get_variable(environment->parent, name);
    if (variable)
    {
        if (variable->immutable)
            pesec_panic("environment::set_variable: variable `%.*s` is immutable", name.size, name.data);
        pesec_value_increase_reference(&value);
        pesec_value_decrease_reference(&variable->value);
        variable->value = value;
        return;
    }

    pesec_panic("environment::set_variable: variable `%.*s` doesn't exists",
                name.size, name.data
    );
}

void pesec_environment_push_variable(
    const pesec_environment_t* environment,
    const pesec_string_view_t name,
    pesec_value_t value,
    const bool immutable
)
{
    pesec_value_increase_reference(&value);
    pesec_variable_table_push(
        environment->variable_table,
        (pesec_variable_t){
            .name = name,
            .value = value,
            .immutable = immutable
        }
    );
}
