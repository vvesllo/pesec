#ifndef PESEC_NUMBER_VALUE_H
#define PESEC_NUMBER_VALUE_H

#include "value.h"
#include "include/common/string_view.h"

typedef struct PESEC_NUMBER_VALUE_STRUCT
{
    double value;
} pesec_number_value_t;

pesec_number_value_t* pesec_number_value_new(double value);

void pesec_number_value_destroy(pesec_number_value_t* number_value);

void pesec_number_value_print(FILE* stream, const pesec_number_value_t* number_value);

double pesec_number_value_get(const pesec_number_value_t* number_value);


bool pesec_number_value_is_truthy(const pesec_number_value_t* number_value);

pesec_value_t pesec_number_value_negative(const pesec_number_value_t* number_value);

pesec_value_t pesec_number_value_add(const pesec_number_value_t* left, pesec_value_t right);

pesec_value_t pesec_number_value_sub(const pesec_number_value_t* left, pesec_value_t right);

pesec_value_t pesec_number_value_mul(const pesec_number_value_t* left, pesec_value_t right);

pesec_value_t pesec_number_value_div(const pesec_number_value_t* left, pesec_value_t right);

pesec_value_t pesec_number_value_mod(const pesec_number_value_t* left, pesec_value_t right);

bool pesec_number_value_equal(const pesec_number_value_t* left, pesec_value_t right);

bool pesec_number_value_less(const pesec_number_value_t* left, pesec_value_t right);

bool pesec_number_value_less_equal(const pesec_number_value_t* left, pesec_value_t right);

bool pesec_number_value_greater(const pesec_number_value_t* left, pesec_value_t right);

bool pesec_number_value_greater_equal(const pesec_number_value_t* left, pesec_value_t right);

pesec_value_t pesec_number_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_NUMBER_VALUE_H
