#include "include/runtime/environment.h"

#include "include/common/panic.h"

pesec_environment_t pesec_environment_new(pesec_environment_t* parent)
{
    pesec_environment_t environment;

    environment.parent = parent;
    environment.variable_table = pesec_variable_table_new();

    return environment;
}

void pesec_environment_destroy(const pesec_environment_t* environment)
{
    pesec_variable_table_destroy(&environment->variable_table);
}

pesec_variable_t* pesec_environment_get_variable(const pesec_environment_t* environment, const pesec_string_view_t name)
{
    pesec_variable_t* variable = pesec_variable_table_find(&environment->variable_table, name);
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
    const pesec_value_t value
)
{
    pesec_variable_t* variable = pesec_variable_table_find(&environment->variable_table, name);
    if (!variable && environment->parent)
        variable = pesec_environment_get_variable(environment->parent, name);
    if (variable)
    {
        if (variable->immutable)
            pesec_panic("environment::set_variable: variable `%.*s` is immutable", name.size, name.data);
        variable->value = value;
        return;
    }

    pesec_panic("environment::set_variable: variable `%.*s` doesn't exists",
                name.size, name.data
    );
}

void pesec_environment_push_variable(
    pesec_environment_t* environment,
    const pesec_string_view_t name,
    const pesec_value_t value,
    const bool immutable
)
{
    pesec_variable_table_push(
        &environment->variable_table,
        (pesec_variable_t){
            .name = name,
            .value = value,
            .immutable = immutable
        }
    );
}
