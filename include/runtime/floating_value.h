#ifndef PESEC_FLOATING_VALUE_H
#define PESEC_FLOATING_VALUE_H

#include "value.h"
#include "include/common/string_view.h"

typedef struct PESEC_FLOATING_VALUE_STRUCT
{
    double value;
} pesec_floating_value_t;

pesec_floating_value_t* pesec_floating_value_new(double value);

void pesec_floating_value_destroy(pesec_floating_value_t* floating_value);

void pesec_floating_value_print(FILE* stream, const pesec_floating_value_t* floating_value);

double pesec_floating_value_get(const pesec_floating_value_t* floating_value);


bool pesec_floating_value_is_truthy(const pesec_floating_value_t* floating_value);

pesec_value_t pesec_floating_value_negative(const pesec_floating_value_t* floating_value);

pesec_value_t pesec_floating_value_add(const pesec_floating_value_t* left, pesec_value_t right);

pesec_value_t pesec_floating_value_sub(const pesec_floating_value_t* left, pesec_value_t right);

pesec_value_t pesec_floating_value_mul(const pesec_floating_value_t* left, pesec_value_t right);

pesec_value_t pesec_floating_value_div(const pesec_floating_value_t* left, pesec_value_t right);

pesec_value_t pesec_floating_value_mod(const pesec_floating_value_t* left, pesec_value_t right);

bool pesec_floating_value_equal(const pesec_floating_value_t* left, pesec_value_t right);

bool pesec_floating_value_less(const pesec_floating_value_t* left, pesec_value_t right);

bool pesec_floating_value_less_equal(const pesec_floating_value_t* left, pesec_value_t right);

bool pesec_floating_value_greater(const pesec_floating_value_t* left, pesec_value_t right);

bool pesec_floating_value_greater_equal(const pesec_floating_value_t* left, pesec_value_t right);

pesec_value_t pesec_floating_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_FLOATING_VALUE_H
