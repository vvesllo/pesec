#include "include/runtime/object_value.h"

#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"

pesec_object_value_t* pesec_object_value_new()
{
    const auto object_value = (pesec_object_value_t*)pesec_malloc(sizeof(pesec_object_value_t));

    object_value->environment = pesec_environment_new(nullptr);

    return object_value;
}

void pesec_object_value_push(
    const pesec_object_value_t* object_value,
    const pesec_string_view_t field,
    const pesec_value_t value,
    const bool immutable
)
{
    pesec_environment_push_variable(object_value->environment, field, value, immutable);
}

void pesec_object_value_set(
    const pesec_object_value_t* object_value,
    const pesec_string_view_t field,
    const pesec_value_t value
)
{
    pesec_environment_set_variable(object_value->environment, field, value);
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

pesec_value_t pesec_object_value_method_handler(
    const pesec_value_t* self,
    const pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    const pesec_variable_t* field_variable = pesec_object_value_get(self->data.object, field);

    if (!field_variable)
    {
        pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                    field.size, field.data);
    }

    return field_variable->value;
}
