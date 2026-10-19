#ifndef PESEC_OBJECT_VALUE_H
#define PESEC_OBJECT_VALUE_H

#include "function_value.h"
#include "include/common/string_view.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VARIABLE_STRUCT pesec_variable_t;

typedef struct
{
    pesec_environment_t* environment;
} pesec_object_value_t;

pesec_object_value_t pesec_object_value_new();

void pesec_object_value_push(
    const pesec_object_value_t* object_value,
    pesec_string_view_t name,
    pesec_value_t value,
    bool immutable
);

pesec_variable_t* pesec_object_value_get(const pesec_object_value_t* object_value, pesec_string_view_t field);

void pesec_object_value_destroy(const pesec_object_value_t* object_value);

#endif // PESEC_OBJECT_VALUE_H
