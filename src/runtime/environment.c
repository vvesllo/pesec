#include "include/runtime/environment.h"

pesec_environment_t pesec_environment_new(pesec_environment_t* parent)
{
    pesec_environment_t environment;

    environment.parent = parent;
    environment.variable_table = pesec_variable_table_new();

    return environment;
}