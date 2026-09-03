#ifndef PESEC_VALUE_H
#define PESEC_VALUE_H

#include <stdio.h>

#include "control_flow.h"
#include "utils/typedefs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct FUNCTION_VALUE_STRUCT function_value_t;
typedef struct STRUCTURE_VALUE_STRUCT structure_value_t;
typedef struct MODULE_VALUE_STRUCT module_value_t;
typedef struct VECTOR_VALUE_STRUCT vector_value_t;
typedef struct STRING_VALUE_STRUCT string_value_t;
typedef struct NUMBER_VALUE_STRUCT number_value_t;

typedef enum
{
    VALUE_TYPE_STRING,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_BOOLEAN,
    VALUE_TYPE_FUNCTION,
    VALUE_TYPE_STRUCTURE,
    VALUE_TYPE_MODULE,
    VALUE_TYPE_VECTOR,
    VALUE_TYPE_NULL,
} value_type_t;

typedef union
{
    bool as_bool;
    number_value_t* as_number;
    string_value_t* as_string;
    function_value_t* as_function;
    structure_value_t* as_structure;
    vector_value_t* as_vector;
    module_value_t* as_module;
    void* as_null;
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

value_t value_new_vector(vector_value_t* data);

value_t value_new_null();

value_t value_new_string_cf(string_value_t* data, control_flow_t control_flow);

value_t value_new_number_cf(number_value_t* data, control_flow_t control_flow);

value_t value_new_boolean_cf(bool data, control_flow_t control_flow);

value_t value_new_function_cf(function_value_t* data, control_flow_t control_flow);

value_t value_new_structure_cf(structure_value_t* data, control_flow_t control_flow);

value_t value_new_module_cf(module_value_t* data, control_flow_t control_flow);

value_t value_new_vector_cf(vector_value_t* data, control_flow_t control_flow);

value_t value_new_null_cf(control_flow_t control_flow);

void value_free(const value_t* value);

void value_increase_reference(value_t* value);

void value_decrease_reference(value_t* value);

bool value_get_boolean(value_t value);

char* value_get_type(value_t value);

value_t value_to_string(value_t value);

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
