#include "include/value.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/vector_value.h"
#include "include/structure_value.h"
#include "include/function_value.h"
#include "include/string_value.h"
#include "include/number_value.h"
#include "include/module_value.h"
#include "include/utils/throw.h"

#define VALUE_TO_STRING_MAX_DEPTH 32

static value_t value_new(void)
{
    return (value_t){
        .reference_count = 1,
        .control_flow = CONTROL_FLOW_NONE
    };
}

value_t value_new_string(string_value_t *data)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_STRING;
    v.data.as_string = data;
    return v;
}

value_t value_new_number(number_value_t *data)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_NUMBER;
    v.data.as_number = data;
    return v;
}

value_t value_new_boolean(const bool data)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_BOOLEAN;
    v.data.as_bool = data;
    return v;
}

value_t value_new_function(function_value_t *data)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_FUNCTION;
    v.data.as_function = data;
    return v;
}

value_t value_new_structure(structure_value_t *d)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_STRUCTURE;
    v.data.as_structure = d;
    return v;
}

value_t value_new_module(module_value_t *data)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_MODULE;
    v.data.as_module = data;
    return v;
}

value_t value_new_vector(vector_value_t *data)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_VECTOR;
    v.data.as_vector = data;
    return v;
}

value_t value_new_null(void)
{
    value_t v = value_new();
    v.type = VALUE_TYPE_NULL;
    v.data.as_null = NULL;
    return v;
}

value_t value_new_string_cf(string_value_t *d, const control_flow_t cf)
{
    value_t v = value_new_string(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_number_cf(number_value_t *d, const control_flow_t cf)
{
    value_t v = value_new_number(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_boolean_cf(const bool d, const control_flow_t cf)
{
    value_t v = value_new_boolean(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_function_cf(function_value_t *d, const control_flow_t cf)
{
    value_t v = value_new_function(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_structure_cf(structure_value_t *d, const control_flow_t cf)
{
    value_t v = value_new_structure(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_module_cf(module_value_t *d, const control_flow_t cf)
{
    value_t v = value_new_module(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_vector_cf(vector_value_t *d, const control_flow_t cf)
{
    value_t v = value_new_vector(d);
    v.control_flow = cf;
    return v;
}

value_t value_new_null_cf(const control_flow_t cf)
{
    value_t v = value_new_null();
    v.control_flow = cf;
    return v;
}

void value_free(const value_t *value)
{
    if (!value) return;

    switch (value->type)
    {
        case VALUE_TYPE_NUMBER: number_value_free(value->data.as_number);
            break;
        case VALUE_TYPE_VECTOR: vector_value_free(value->data.as_vector);
            break;
        case VALUE_TYPE_STRING: string_value_free(value->data.as_string);
            break;
        case VALUE_TYPE_STRUCTURE: structure_value_free(value->data.as_structure);
            break;
        case VALUE_TYPE_FUNCTION: function_value_free(value->data.as_function);
            break;
        case VALUE_TYPE_MODULE: module_value_free(value->data.as_module);
            break;
        default: break;
    }
}

/* ВНИМАНИЕ: до переноса счётчика в heap-объекты (см. заметку ниже) эти две
   функции корректны только в модели "один value_t — один владелец-слот". */
void value_increase_reference(value_t *value)
{
    ++value->reference_count;
}

void value_decrease_reference(value_t *value)
{
    if (value->reference_count == 0)
        return;
    if (--value->reference_count == 0)
        value_free(value);
}

bool value_get_boolean(const value_t value)
{
    switch (value.type)
    {
        case VALUE_TYPE_NUMBER:
            /* было: compare(n, number_value_zero()) — аллокация и утечка на каждый вызов */
            return number_value_mantissa_significant_size(value.data.as_number->mantissa) != 0;
        case VALUE_TYPE_STRING:
            return value.data.as_string->size != 0;
        case VALUE_TYPE_BOOLEAN:
            return value.data.as_bool;
        case VALUE_TYPE_FUNCTION:
        case VALUE_TYPE_VECTOR:
        case VALUE_TYPE_MODULE:
        case VALUE_TYPE_STRUCTURE:
            return true;
        case VALUE_TYPE_NULL:
            return false;
    }

    THROW("Value type '%d' is not a valid value type\n", value.type);
}

const char *value_get_type(const value_t value)
{
    switch (value.type)
    {
        case VALUE_TYPE_NUMBER: return "number";
        case VALUE_TYPE_STRING: return "string";
        case VALUE_TYPE_BOOLEAN: return "boolean";
        case VALUE_TYPE_FUNCTION: return "function";
        case VALUE_TYPE_STRUCTURE: return "structure";
        case VALUE_TYPE_VECTOR: return "vector";
        case VALUE_TYPE_MODULE: return "module";
        case VALUE_TYPE_NULL: return "null";
    }

    THROW("Value type '%d' is not a valid value type\n", value.type);
}

static void string_value_append_cstr(string_value_t *out, const char *str)
{
    for (const char *p = str; *p; ++p)
        string_value_push_back(out, *p);
}

static void string_value_append_string(string_value_t *out, const string_value_t *src)
{
    for (u64_t i = 0; i < src->size; ++i)
        string_value_push_back(out, src->data[i]);
}

static void value_string_to_string(string_value_t *out, const string_value_t *data)
{
    string_value_append_string(out, data);
}

static void value_number_to_string(string_value_t *out, const number_value_t *data)
{
    /* линейная печать из нового number_value.c; старый ручной разбор
       по полубайтам удалён — он несовместим с лимбовым представлением */
    char *text = number_value_to_string(data);
    if (!text)
    {
        string_value_append_cstr(out, "<error>");
        return;
    }
    string_value_append_cstr(out, text);
    free(text);
}

static void value_boolean_to_string(string_value_t *out, const bool data)
{
    string_value_append_cstr(out, data ? "true" : "false");
}

static void value_null_to_string(string_value_t *out)
{
    string_value_append_cstr(out, "null");
}

static void value_function_to_string(string_value_t *out, const function_value_t *data)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "<function:%p>", (const void *) data);
    string_value_append_cstr(out, buffer);
}

static void value_module_to_string(string_value_t *out, const module_value_t *data)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "<module:%p>", (const void *) data);
    string_value_append_cstr(out, buffer);
}

static void value_structure_to_string(string_value_t *out, const structure_value_t *data)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "<structure:%p>", (const void *) data);
    string_value_append_cstr(out, buffer);
}

static value_t value_to_string_depth(const value_t value, const int depth);

static void value_vector_to_string(string_value_t *out, const vector_value_t *data, const int depth)
{
    if (!data)
    {
        string_value_append_cstr(out, "null");
        return;
    }

    if (depth > VALUE_TO_STRING_MAX_DEPTH)
    {
        /* защита от v = [v] */
        string_value_append_cstr(out, "[...]");
        return;
    }

    string_value_push_back(out, '[');

    for (u64_t i = 0; i < data->size; ++i)
    {
        if (i > 0)
            string_value_append_cstr(out, ", ");

        value_t elem_str = value_to_string_depth(data->values[i], depth + 1);
        if (elem_str.type == VALUE_TYPE_STRING)
        {
            if (data->values[i].type == VALUE_TYPE_STRING) string_value_push_back(out, '"');
            string_value_append_string(out, elem_str.data.as_string);
            if (data->values[i].type == VALUE_TYPE_STRING) string_value_push_back(out, '"');
        } else
            string_value_append_cstr(out, "<error>");

        value_decrease_reference(&elem_str);
    }

    string_value_push_back(out, ']');
}

static value_t value_to_string_depth(const value_t value, const int depth)
{
    string_value_t *result = string_value_new();

    switch (value.type)
    {
        case VALUE_TYPE_STRING: value_string_to_string(result, value.data.as_string);
            break;
        case VALUE_TYPE_NUMBER: value_number_to_string(result, value.data.as_number);
            break;
        case VALUE_TYPE_BOOLEAN: value_boolean_to_string(result, value.data.as_bool);
            break;
        case VALUE_TYPE_FUNCTION: value_function_to_string(result, value.data.as_function);
            break;
        case VALUE_TYPE_STRUCTURE: value_structure_to_string(result, value.data.as_structure);
            break;
        case VALUE_TYPE_VECTOR: value_vector_to_string(result, value.data.as_vector, depth);
            break;
        case VALUE_TYPE_MODULE: value_module_to_string(result, value.data.as_module);
            break;
        case VALUE_TYPE_NULL: value_null_to_string(result);
            break;
        default: string_value_append_cstr(result, "<unknown>");
            break;
    }

    return value_new_string(result);
}

value_t value_to_string(const value_t value)
{
    return value_to_string_depth(value, 0);
}

value_t value_operation_not(const value_t value)
{
    return value_new_boolean(!value_get_boolean(value));
}

value_t value_operation_negate(const value_t value)
{
    if (value.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_negate(value.data.as_number));

    THROW("Unsupported operator '-' for '%s'\n", value_get_type(value));
}

value_t value_operation_add(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_add(left.data.as_number, right.data.as_number));
    if (left.type == VALUE_TYPE_STRING && right.type == VALUE_TYPE_STRING)
        return value_new_string(string_value_concat(left.data.as_string, right.data.as_string));
    if (left.type == VALUE_TYPE_VECTOR && right.type == VALUE_TYPE_VECTOR)
        return value_new_vector(vector_value_concat(left.data.as_vector, right.data.as_vector));

    THROW("Unsupported operator '+' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_sub(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_sub(left.data.as_number, right.data.as_number));

    THROW("Unsupported operator '-' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_mul(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_mul(left.data.as_number, right.data.as_number));

    THROW("Unsupported operator '*' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_div(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_div(left.data.as_number, right.data.as_number));

    THROW("Unsupported operator '/' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_floor_div(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_floor_div(left.data.as_number, right.data.as_number));

    THROW("Unsupported operator '//' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_pow(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_pow(left.data.as_number, right.data.as_number));

    THROW("Unsupported operator '**' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_equals(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_boolean(number_value_compare(left.data.as_number, right.data.as_number) == 0);
    if (left.type == VALUE_TYPE_STRING && right.type == VALUE_TYPE_STRING)
        return value_new_boolean(string_value_equals(left.data.as_string, right.data.as_string));
    if (left.type == VALUE_TYPE_BOOLEAN && right.type == VALUE_TYPE_BOOLEAN)
        return value_new_boolean(left.data.as_bool == right.data.as_bool);

    /* NB: несогласованность с '<' (тот молча возвращает false) — семантика ниже */
    THROW("Unsupported operator '==' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}

value_t value_operation_less_or_equals(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_boolean(number_value_compare(left.data.as_number, right.data.as_number) <= 0);
    return value_new_boolean(false);
}

value_t value_operation_less(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_boolean(number_value_compare(left.data.as_number, right.data.as_number) < 0);
    return value_new_boolean(false);
}

value_t value_operation_greater_or_equals(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_boolean(number_value_compare(left.data.as_number, right.data.as_number) >= 0);
    return value_new_boolean(false);
}

value_t value_operation_greater(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_boolean(number_value_compare(left.data.as_number, right.data.as_number) > 0);
    return value_new_boolean(false);
}

value_t value_operation_compare(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
    {
        const short c = number_value_compare(left.data.as_number, right.data.as_number);
        return value_new_number(number_value_from_long_double(c < 0 ? -1 : (c > 0 ? 1 : 0)));
    }

    THROW("Unsupported operator '<=>' for '%s' and '%s'\n", value_get_type(left), value_get_type(right));
}
