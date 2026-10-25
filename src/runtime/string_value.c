#include "include/runtime/string_value.h"

#include <string.h>

#include "include/ast/node.h"
#include "include/common/alloc.h"
#include "include/common/panic.h"
#include "include/runtime/environment.h"
#include "include/runtime/number_value.h"


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

static int pesec_string_value_compare(const pesec_string_value_t* left, const pesec_string_value_t* right)
{
    const char* left_items = pesec_string_value_get_items_const(left);
    const char* right_items = pesec_string_value_get_items_const(right);

    const size_t min_size = left->size < right->size ? left->size : right->size;
    const int cmp = memcmp(left_items, right_items, min_size);

    if (cmp < 0) return -1;
    if (cmp > 0) return 1;

    if (left->size < right->size) return -1;
    if (left->size > right->size) return 1;
    return 0;
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

char* pesec_string_value_get_items(pesec_string_value_t* string_value)
{
    return string_value->is_heap ? string_value->items.heap : string_value->items._inline;
}

const char* pesec_string_value_get_items_const(const pesec_string_value_t* string_value)
{
    return string_value->is_heap ? string_value->items.heap : string_value->items._inline;
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

void pesec_string_value_print(FILE* stream, const pesec_string_value_t* string_value)
{
    fwrite(pesec_string_value_get_items_const(string_value), 1, string_value->size, stream);
}

bool pesec_string_value_is_truthy(const pesec_string_value_t* string_value)
{
    return string_value->size > 0;
}

pesec_value_t pesec_string_value_negative(const pesec_string_value_t* string_value)
{
    pesec_panic("string_value::negative: unsupported");
}

pesec_value_t pesec_string_value_add(const pesec_string_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_STRING)
        pesec_panic("string_value::add: right operand must be string");

    const pesec_string_value_t* right_string = right.data.string;
    pesec_string_value_t* result = pesec_string_value_new();

    pesec_string_value_append(
        result,
        pesec_string_view_new(pesec_string_value_get_items_const(left), left->size)
    );

    pesec_string_value_append(
        result,
        pesec_string_view_new(pesec_string_value_get_items_const(right_string), right_string->size)
    );

    return pesec_value_as_string(result);
}

pesec_value_t pesec_string_value_sub(const pesec_string_value_t* left, const pesec_value_t right)
{
    pesec_panic("string_value::sub: unsupported");
}

pesec_value_t pesec_string_value_mul(const pesec_string_value_t* left, const pesec_value_t right)
{
    pesec_panic("string_value::mul: unsupported");
}

pesec_value_t pesec_string_value_div(const pesec_string_value_t* left, const pesec_value_t right)
{
    pesec_panic("string_value::div: unsupported");
}

pesec_value_t pesec_string_value_mod(const pesec_string_value_t* left, const pesec_value_t right)
{
    pesec_panic("string_value::mod: unsupported");
}

bool pesec_string_value_equal(const pesec_string_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_STRING)
        pesec_panic("string_value::equal: right operand must be string");

    return pesec_string_value_compare(left, right.data.string) == 0;
}

bool pesec_string_value_less(const pesec_string_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_STRING)
        pesec_panic("string_value::less: right operand must be string");

    return pesec_string_value_compare(left, right.data.string) < 0;
}

bool pesec_string_value_less_equal(const pesec_string_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_STRING)
        pesec_panic("string_value::less_equal: right operand must be string");

    return pesec_string_value_compare(left, right.data.string) <= 0;
}

bool pesec_string_value_greater(const pesec_string_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_STRING)
        pesec_panic("string_value::greater: right operand must be string");

    return pesec_string_value_compare(left, right.data.string) > 0;
}

bool pesec_string_value_greater_equal(const pesec_string_value_t* left, const pesec_value_t right)
{
    if (right.type != PESEC_VALUE_TYPE_STRING)
        pesec_panic("string_value::greater_equal: right operand must be string");

    return pesec_string_value_compare(left, right.data.string) >= 0;
}

static pesec_value_t pesec_string_value_method_size(
    pesec_value_t* self,
    pesec_environment_t* environment
)
{
    pesec_value_t size = pesec_value_as_number(pesec_number_value_new((double)self->data.string->size));
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
        default:
            pesec_panic("string::push_back: `value` should be a string or integer");
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

    pesec_panic("field_access_node::evaluate: field `%.*s` is not exists in object",
                field.size, field.data);
}
