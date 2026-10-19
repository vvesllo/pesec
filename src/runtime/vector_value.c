#include "include/runtime/vector_value.h"

#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"

pesec_vector_value_t* pesec_vector_value_new()
{
    const auto vector_value = (pesec_vector_value_t*)pesec_malloc(sizeof(pesec_vector_value_t));

    vector_value->size = 0;
    vector_value->capacity = 64;
    vector_value->items = (pesec_value_t*)pesec_calloc(vector_value->capacity, sizeof(pesec_value_t));

    return vector_value;
}

void pesec_vector_value_push(pesec_vector_value_t* vector_value, pesec_value_t value)
{
    if (vector_value->size >= vector_value->capacity)
    {
        vector_value->capacity <<= 1;
        vector_value->items = (pesec_value_t*)pesec_realloc(vector_value->items,
                                                            vector_value->capacity * sizeof(pesec_value_t));
    }

    vector_value->items[vector_value->size] = value;
    vector_value->size += 1;
}

void pesec_vector_value_destroy(const pesec_vector_value_t* vector_value)
{
    pesec_free(vector_value->items);
}

static pesec_value_t pesec_vector_value_method_size(
    pesec_value_t* self,
    pesec_environment_t* environment
)
{
    pesec_value_t size = pesec_value_i64((int64_t)self->data.vector->size);
    size.control_flow = PESEC_VALUE_CONTROL_FLOW_RETURN;
    return size;
}

static pesec_value_t pesec_vector_value_method_push_back(
    pesec_value_t* self,
    pesec_environment_t* environment
)
{
    const pesec_variable_t* variable = pesec_environment_get_variable(
        environment,
        pesec_string_view_from("value")
    );
    if (!variable)
        pesec_panic("vector::push_back: missing `value` argument");

    pesec_vector_value_push(self->data.vector, variable->value);
    return pesec_value_none();
}

pesec_value_t pesec_vector_value_method_handler(
    pesec_value_t* self,
    const pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    if (pesec_string_view_equals_cstr(field, "size"))
        return pesec_value_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_METHOD,
            (pesec_function_value_data_t){
                .method.self = *self,
                .method.method = pesec_vector_value_method_size,
            },
            pesec_parameter_vector_new(),
            environment
        ));
    if (pesec_string_view_equals_cstr(field, "push_back"))
        return pesec_value_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_METHOD,
            (pesec_function_value_data_t){
                .method.self = *self,
                .method.method = pesec_vector_value_method_push_back,
            },
            pesec_parameter_vector_from(
                (const pesec_parameter_t[]){
                    (pesec_parameter_t){.name = pesec_string_view_from("value")}
                },
                1
            ),
            environment
        ));

    pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                field.size, field.data);
}
