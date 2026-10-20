#ifndef PESEC_ENVIRONMENT_H
#define PESEC_ENVIRONMENT_H


#include "include/runtime/variable_table.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;

typedef struct PESEC_ENVIRONMENT_STRUCT
{
    pesec_environment_t* parent;
    pesec_variable_table_t* variable_table;
} pesec_environment_t;

pesec_environment_t* pesec_environment_new(pesec_environment_t* parent);

void pesec_environment_destroy(pesec_environment_t* environment);

pesec_variable_t* pesec_environment_get_variable(const pesec_environment_t* environment, pesec_string_view_t name);

void pesec_environment_set_variable(
    const pesec_environment_t* environment,
    pesec_string_view_t name,
    pesec_value_t value
);

void pesec_environment_push_variable(
    const pesec_environment_t* environment,
    pesec_string_view_t name,
    pesec_value_t value,
    bool immutable
);

#endif // PESEC_ENVIRONMENT_H
