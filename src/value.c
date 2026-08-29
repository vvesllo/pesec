#include "include/value.h"

#include <stdio.h>
#include <stdlib.h>

#include "include/array_value.h"
#include "include/structure_value.h"
#include "include/function_value.h"
#include "include/module_value.h"
#include "include/utils/throw.h"


value_t value_new()
{
    return (value_t){
        .reference_count = 1,
        .control_flow = CONTROL_FLOW_NONE
    };
}

value_t value_new_string(string_value_t *data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_STRING;
    value.data.as_string = data;
    return value;
}

value_t value_new_number(number_value_t *data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_NUMBER;
    value.data.as_number = data;
    return value;
}

value_t value_new_boolean(const bool data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_BOOLEAN;
    value.data.as_bool = data;
    return value;
}

value_t value_new_function(function_value_t *data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_FUNCTION;
    value.data.as_function = data;
    return value;
}

value_t value_new_structure(structure_value_t *data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_STRUCTURE;
    value.data.as_structure = data;
    return value;
}

value_t value_new_module(module_value_t *data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_MODULE;
    value.data.as_module = data;
    return value;
}

value_t value_new_array(array_value_t *data)
{
    value_t value = value_new();
    value.type = VALUE_TYPE_ARRAY;
    value.data.as_array = data;
    return value;
}

void value_free(const value_t *value)
{
    switch (value->type)
    {
        case VALUE_TYPE_NUMBER: number_value_free(value->data.as_number); break;
        case VALUE_TYPE_ARRAY: array_value_free(value->data.as_array); break;
        case VALUE_TYPE_STRING: string_value_free(value->data.as_string); break;
        case VALUE_TYPE_STRUCTURE: structure_value_free(value->data.as_structure); break;
        case VALUE_TYPE_FUNCTION: function_value_free(value->data.as_function); break;
        case VALUE_TYPE_MODULE: module_value_free(value->data.as_module); break;
        default: break;
    }
}

void value_increase_reference(value_t *value)
{
    ++value->reference_count;
}

void value_decrease_reference(value_t *value)
{
    --value->reference_count;
    if (value->reference_count <= 0)
    {
        value_free(value);
    }
}

bool value_get_boolean(const value_t value)
{
    switch (value.type)
    {
        case VALUE_TYPE_NUMBER: return value.data.as_number->decimal->size;
        case VALUE_TYPE_STRING: return value.data.as_string->size != 0;
        case VALUE_TYPE_BOOLEAN: return value.data.as_bool;
        case VALUE_TYPE_FUNCTION:
        case VALUE_TYPE_ARRAY:
        case VALUE_TYPE_MODULE:
        case VALUE_TYPE_STRUCTURE: return true;
    }

    THROW("Value type '%d' is not a valid value type\n", value.type);
}

char *value_get_type(const value_t value)
{
    switch (value.type)
    {
        case VALUE_TYPE_NUMBER: return "number";
        case VALUE_TYPE_STRING: return "string";
        case VALUE_TYPE_BOOLEAN: return "boolean";
        case VALUE_TYPE_FUNCTION: return "function";
        case VALUE_TYPE_STRUCTURE: return "structure";
        case VALUE_TYPE_ARRAY: return "array";
        case VALUE_TYPE_MODULE: return "module";
    }

    THROW("Value type '%d' is not a valid value type\n", value.type);
}

void value_print(FILE *stream, const value_t value)
{
    switch (value.type)
    {
        case VALUE_TYPE_STRING: value_print_string(stream, value.data.as_string);
            return;
        case VALUE_TYPE_NUMBER: value_print_number(stream, value.data.as_number);
            return;
        case VALUE_TYPE_BOOLEAN: value_print_boolean(stream, value.data.as_bool);
            return;
        case VALUE_TYPE_FUNCTION: value_print_function(stream, value.data.as_function);
            return;
        case VALUE_TYPE_STRUCTURE: value_print_structure(stream, value.data.as_structure);
            return;
        case VALUE_TYPE_ARRAY: value_print_array(stream, value.data.as_array);
            return;
        case VALUE_TYPE_MODULE: value_print_module(stream, value.data.as_module);
            return;
    }

    THROW("Value type '%d' is not a valid value type\n", value.type);
}

void value_print_string(FILE *stream, const string_value_t *value)
{
    fprintf(stream, "%s", value->data);
}

void value_print_number(FILE *stream, const number_value_t *value)
{
    for (ull_t i = 0; i < value->decimal->length; ++i)
        fprintf(stream, "%llu", value->decimal->data[i]);

    if (value->fraction->length > 0)
    {
        fprintf(stream, ".");
        for (ull_t i = 0; i < value->fraction->length; ++i)
            fprintf(stream, "%llu", value->fraction->data[i]);
    }
}

void value_print_boolean(FILE *stream, const bool value)
{
    fprintf(stream, value ? "true" : "false");
}

void value_print_function(FILE *stream, const function_value_t *value)
{
    fprintf(stream, "<function:%p>", &value);
}

void value_print_structure(FILE *stream, const structure_value_t *value)
{
    fprintf(stream, "<structure:%p>", &value);
}

void value_print_module(FILE *stream, const module_value_t *value)
{
    fprintf(stream, "<module:%p>", &value);
}

void value_print_array(FILE *stream, const array_value_t *value)
{
    fprintf(stream, "[");
    for (ull_t i = 0; i < value->size; ++i)
    {
        if (i > 0) fprintf(stream, ", ");
        value_print(stream, value->values[i]);
    }
    fprintf(stream, "]");
}

value_t value_operation_add(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_add(left.data.as_number, right.data.as_number));
    if (left.type == VALUE_TYPE_STRING && right.type == VALUE_TYPE_STRING)
        return value_new_string(string_value_concat(left.data.as_string, right.data.as_string));

    THROW("Unsupported operator '+' for types '%d' and '%d'\n", left.type, right.type);
}

value_t value_operation_sub(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_sub(left.data.as_number, right.data.as_number));
    THROW("Unsupported operator '-' for types '%d' and '%d'\n", left.type, right.type);
}

value_t value_operation_mul(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_number(number_value_mul(left.data.as_number, right.data.as_number));
    THROW("Unsupported operator '*' for types '%d' and '%d'\n", left.type, right.type);
}

value_t value_operation_div(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
    {
        return value_new_number(number_value_div(left.data.as_number, right.data.as_number));
    }
    THROW("Unsupported operator '*' for types '%d' and '%d'\n", left.type, right.type);
}

value_t value_operation_equals(const value_t left, const value_t right)
{
    if (left.type == VALUE_TYPE_NUMBER && right.type == VALUE_TYPE_NUMBER)
        return value_new_boolean(number_value_compare(left.data.as_number, right.data.as_number) == 0);
    if (left.type == VALUE_TYPE_STRING && right.type == VALUE_TYPE_STRING)
        return value_new_boolean(string_value_equals(left.data.as_string, right.data.as_string));
    return value_new_boolean(false);
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
