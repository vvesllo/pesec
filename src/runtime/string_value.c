#include "include/runtime/string_value.h"

#include <string.h>

#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"

static char* pesec_string_value_get_items(pesec_string_value_t* string_value)
{
    return string_value->is_heap ? string_value->items.heap : string_value->items._inline;
}

static const char* pesec_string_value_get_items_const(const pesec_string_value_t* string_value)
{
    return string_value->is_heap ? string_value->items.heap : string_value->items._inline;
}

static void pesec_string_value_reserve(pesec_string_value_t* string_value, const size_t min_capacity)
{
    if (min_capacity <= string_value->capacity)
        return;

    size_t new_capacity = string_value->capacity;
    while (new_capacity < min_capacity)
        new_capacity <<= 1;

    if (string_value->is_heap)
    {
        string_value->items.heap = (char*)pesec_realloc(string_value->items.heap, new_capacity);
    }
    else
    {
        const auto new_items = (char*)pesec_malloc(new_capacity);
        memcpy(new_items, string_value->items._inline, string_value->size);
        string_value->items.heap = new_items;
        string_value->is_heap = true;
    }

    string_value->capacity = new_capacity;
}

pesec_string_value_t* pesec_string_value_new()
{
    const auto string_value = (pesec_string_value_t*)pesec_malloc(sizeof(pesec_string_value_t));

    string_value->is_heap = false;
    string_value->size = 0;
    string_value->capacity = PESEC_VARIABLE_TABLE_ITEMS_INLINE_SIZE;
    memset(string_value->items._inline, 0, sizeof(string_value->items._inline));

    return string_value;
}

pesec_string_value_t* pesec_string_value_from(const pesec_string_view_t string_view)
{
    pesec_string_value_t* string_value = pesec_string_value_new();
    pesec_string_value_append(string_value, string_view);
    return string_value;
}

void pesec_string_value_push(pesec_string_value_t* string_value, const char c)
{
    pesec_string_value_reserve(string_value, string_value->size + 1);

    char* items = pesec_string_value_get_items(string_value);
    items[string_value->size] = c;
    string_value->size += 1;
}

void pesec_string_value_append(pesec_string_value_t* string_value, const pesec_string_view_t string_view)
{
    if (string_view.size == 0)
        return;

    pesec_string_value_reserve(string_value, string_value->size + string_view.size);

    char* items = pesec_string_value_get_items(string_value);
    memcpy(items + string_value->size, string_view.data, string_view.size);
    string_value->size += string_view.size;
}

void pesec_string_value_destroy(pesec_string_value_t* string_value)
{
    if (string_value->is_heap)
    {
        pesec_free(string_value->items.heap);
    }
    pesec_free(string_value);
}

static pesec_value_t pesec_string_value_method_size(
    pesec_value_t* self,
    pesec_environment_t* environment
)
{
    pesec_value_t size = pesec_value_as_i64((int64_t)self->data.string->size);
    size.control_flow = PESEC_VALUE_CONTROL_FLOW_RETURN;
    return size;
}

static pesec_value_t pesec_string_value_method_push_back(
    pesec_value_t* self,
    pesec_environment_t* environment
)
{
    const pesec_variable_t* variable = pesec_environment_get_variable(
        environment,
        pesec_string_view_from("value")
    );
    if (!variable)
        pesec_panic("string::push_back: missing `value` argument");

    switch (variable->value.type)
    {
        case PESEC_VALUE_TYPE_STRING: {
            const pesec_string_value_t* other = variable->value.data.string;
            pesec_string_value_append(
                self->data.string,
                pesec_string_view_new(pesec_string_value_get_items_const(other), other->size)
            );
            break;
        }
        case PESEC_VALUE_TYPE_I64: {
            pesec_string_value_push(self->data.string, (char)variable->value.data.i64);
            break;
        }
        default:
            pesec_panic("string::push_back: `value` should be a string or i64");
    }

    return pesec_value_as_none();
}

pesec_value_t pesec_string_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
)
{
    if (pesec_string_view_equals_cstr(field, "size"))
        return pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_METHOD,
            (pesec_function_value_data_t){
                .method.self = *self,
                .method.method = pesec_string_value_method_size,
            },
            pesec_parameter_vector_new(),
            environment
        ));

    if (pesec_string_view_equals_cstr(field, "push_back"))
        return pesec_value_as_function(pesec_function_value_new(
            PESEC_FUNCTION_VALUE_TYPE_METHOD,
            (pesec_function_value_data_t){
                .method.self = *self,
                .method.method = pesec_string_value_method_push_back,
            },
            pesec_parameter_vector_from(
                (const pesec_parameter_t[]){
                    (pesec_parameter_t){.name = pesec_string_view_from("value")}
                },
                1
            ),
            environment
        ));
}
