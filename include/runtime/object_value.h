#ifndef PESEC_OBJECT_VALUE_H
#define PESEC_OBJECT_VALUE_H

#include "function_value.h"
#include "include/common/string_view.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;
typedef struct PESEC_VARIABLE_STRUCT pesec_variable_t;

typedef struct PESEC_OBJECT_VALUE_STRUCT
{
    pesec_environment_t* environment;
} pesec_object_value_t;

pesec_object_value_t* pesec_object_value_new();

void pesec_object_value_push(
    const pesec_object_value_t* object_value,
    pesec_string_view_t field,
    pesec_value_t value,
    bool immutable
);

void pesec_object_value_set(
    const pesec_object_value_t* object_value,
    pesec_string_view_t field,
    pesec_value_t value
);

pesec_variable_t* pesec_object_value_get(const pesec_object_value_t* object_value, pesec_string_view_t field);

void pesec_object_value_destroy(const pesec_object_value_t* object_value);

pesec_value_t pesec_object_value_method_handler(
    const pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_OBJECT_VALUE_H
