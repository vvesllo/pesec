#ifndef PESEC_ENVIRONMENT_H
#define PESEC_ENVIRONMENT_H


#include "include/runtime/variable_table.h"

typedef struct PESEC_ENVIRONMENT_STRUCT pesec_environment_t;

typedef struct PESEC_ENVIRONMENT_STRUCT
{
    pesec_environment_t* parent;
    pesec_variable_table_t variable_table;
} pesec_environment_t;

pesec_environment_t pesec_environment_new(pesec_environment_t* parent);

#endif // PESEC_ENVIRONMENT_H