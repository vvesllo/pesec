#ifndef PESEC_VALUE_H
#define PESEC_VALUE_H

#include <inttypes.h>
#include <stdio.h>

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VALUE_STRUCT pesec_value_t;

typedef struct PESEC_FUNCTION_VALUE_STRUCT pesec_function_value_t;
typedef struct PESEC_VECTOR_VALUE_STRUCT pesec_vector_value_t;
typedef struct PESEC_OBJECT_VALUE_STRUCT pesec_object_value_t;
typedef struct PESEC_STRING_VALUE_STRUCT pesec_string_value_t;
typedef struct PESEC_NUMBER_VALUE_STRUCT pesec_number_value_t;


typedef enum
{
    PESEC_VALUE_TYPE_NONE,
    PESEC_VALUE_TYPE_BOOLEAN,
    PESEC_VALUE_TYPE_NUMBER,
    PESEC_VALUE_TYPE_FUNCTION,
    PESEC_VALUE_TYPE_VECTOR,
    PESEC_VALUE_TYPE_OBJECT,
    PESEC_VALUE_TYPE_STRING,
} pesec_value_type_t;

typedef enum
{
    PESEC_VALUE_CONTROL_FLOW_NONE,
    PESEC_VALUE_CONTROL_FLOW_CONTINUE,
    PESEC_VALUE_CONTROL_FLOW_BREAK,
    PESEC_VALUE_CONTROL_FLOW_RETURN,
} pesec_value_control_flow_t;

typedef union
{
    bool boolean;
    pesec_number_value_t* number;
    pesec_function_value_t* function;
    pesec_vector_value_t* vector;
    pesec_object_value_t* object;
    pesec_string_value_t* string;
} pesec_value_data_t;

typedef struct PESEC_VALUE_STRUCT
{
    pesec_value_type_t type;
    pesec_value_data_t data;
    pesec_value_control_flow_t control_flow;
    int64_t reference_count;
} pesec_value_t;

void pesec_value_print(FILE* stream, pesec_value_t value);

pesec_value_t pesec_value_as_boolean(bool data);

pesec_value_t pesec_value_as_number(pesec_number_value_t* data);

pesec_value_t pesec_value_as_function(pesec_function_value_t* data);

pesec_value_t pesec_value_as_vector(pesec_vector_value_t* data);

pesec_value_t pesec_value_as_object(pesec_object_value_t* data);

pesec_value_t pesec_value_as_string(pesec_string_value_t* data);

pesec_value_t pesec_value_as_none();


pesec_value_t pesec_value_as_boolean_with(bool data, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_value_as_number_with(pesec_number_value_t* data, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_value_as_function_with(pesec_function_value_t* data, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_value_as_vector_with(pesec_vector_value_t* data, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_value_as_object_with(pesec_object_value_t* data, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_value_as_string_with(pesec_string_value_t* data, pesec_value_control_flow_t control_flow);

pesec_value_t pesec_value_as_none_with(pesec_value_control_flow_t control_flow);


void pesec_value_increase_reference(pesec_value_t* value);

void pesec_value_decrease_reference(pesec_value_t* value);


bool pesec_value_is_truthy(pesec_value_t value);


pesec_value_t pesec_value_negative(pesec_value_t value);

pesec_value_t pesec_value_add(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_sub(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_mul(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_div(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_mod(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_equal(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_less(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_less_equal(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_greater(pesec_value_t left, pesec_value_t right);

pesec_value_t pesec_value_greater_equal(pesec_value_t left, pesec_value_t right);

#endif // PESEC_VALUE_H
