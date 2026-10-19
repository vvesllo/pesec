#include "include/runtime/object_value.h"

#include "include/common/panic.h"
#include "include/runtime/environment.h"

pesec_object_value_t pesec_object_value_new()
{
    pesec_object_value_t object_value;

    object_value.environment = pesec_environment_new(nullptr);

    return object_value;
}

void pesec_object_value_push(
    const pesec_object_value_t* object_value,
    const pesec_string_view_t name,
    const pesec_value_t value,
    const bool immutable
)
{
    pesec_environment_push_variable(object_value->environment, name, value, immutable);
}

pesec_variable_t* pesec_object_value_get(const pesec_object_value_t* object_value, const pesec_string_view_t field)
{
    pesec_variable_t* variable = pesec_environment_get_variable(object_value->environment, field);

    if (!variable)
        pesec_panic("object::value_get: field `%.*s` doesn't exists",
                    field.size, field.data);

    return variable;
}

void pesec_object_value_destroy(const pesec_object_value_t* object_value)
{
    pesec_environment_destroy(object_value->environment);
}
