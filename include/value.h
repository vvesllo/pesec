#ifndef PESEC_VALUE_H
#define PESEC_VALUE_H

#include <stdio.h>

#include "number_value.h"
#include "string_value.h"
#include "control_flow.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct FUNCTION_VALUE_STRUCT function_value_t;
typedef struct STRUCTURE_VALUE_STRUCT structure_value_t;
typedef struct MODULE_VALUE_STRUCT module_value_t;
typedef struct ARRAY_VALUE_STRUCT array_value_t;

typedef enum
{
    VALUE_TYPE_STRING,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_BOOLEAN,
    VALUE_TYPE_FUNCTION,
    VALUE_TYPE_STRUCTURE,
    VALUE_TYPE_MODULE,
    VALUE_TYPE_ARRAY,
} value_type_t;

typedef union
{
    bool as_bool;
    number_value_t* as_number;
    string_value_t* as_string;
    function_value_t* as_function;
    structure_value_t* as_structure;
    array_value_t* as_array;
    module_value_t* as_module;
} value_value_t;

typedef struct VALUE_STRUCT
{
    ull_t reference_count;
    value_type_t type;
    value_value_t data;
    control_flow_t control_flow;
} value_t;

value_t value_new_string(string_value_t* data);

value_t value_new_number(number_value_t* data);

value_t value_new_boolean(bool data);

value_t value_new_function(function_value_t* data);

value_t value_new_structure(structure_value_t* data);

value_t value_new_module(module_value_t* data);

value_t value_new_array(array_value_t* data);

value_t value_new_string_cf(string_value_t* data, control_flow_t control_flow);

value_t value_new_number_cf(number_value_t* data, control_flow_t control_flow);

value_t value_new_boolean_cf(bool data, control_flow_t control_flow);

value_t value_new_function_cf(function_value_t* data, control_flow_t control_flow);

value_t value_new_structure_cf(structure_value_t* data, control_flow_t control_flow);

value_t value_new_module_cf(module_value_t* data, control_flow_t control_flow);

value_t value_new_array_cf(array_value_t* data, control_flow_t control_flow);

void value_free(const value_t* value);

void value_increase_reference(value_t* value);

void value_decrease_reference(value_t* value);

bool value_get_boolean(value_t value);

char* value_get_type(value_t value);

void value_print(FILE* stream, value_t value);

void value_print_string(FILE* stream, const string_value_t* value);

void value_print_number(FILE* stream, const number_value_t* value);

void value_print_boolean(FILE* stream, bool value);

void value_print_function(FILE* stream, const function_value_t* value);

void value_print_structure(FILE* stream, const structure_value_t* value);

void value_print_module(FILE* stream, const module_value_t* value);

void value_print_array(FILE* stream, const array_value_t* value);

value_t value_operation_not(value_t value);

value_t value_operation_negate(value_t value);

value_t value_operation_add(value_t left, value_t right);

value_t value_operation_sub(value_t left, value_t right);

value_t value_operation_mul(value_t left, value_t right);

value_t value_operation_div(value_t left, value_t right);

value_t value_operation_floor_div(value_t left, value_t right);

value_t value_operation_pow(value_t left, value_t right);

value_t value_operation_equals(value_t left, value_t right);

value_t value_operation_not_equals(value_t left, value_t right);

value_t value_operation_less_or_equals(value_t left, value_t right);

value_t value_operation_less(value_t left, value_t right);

value_t value_operation_greater_or_equals(value_t left, value_t right);

value_t value_operation_greater(value_t left, value_t right);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_VALUE_H
