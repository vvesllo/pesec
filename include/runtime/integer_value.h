#ifndef PESEC_INTEGER_VALUE_H
#define PESEC_INTEGER_VALUE_H

#include "value.h"
#include "include/common/string_view.h"

typedef struct PESEC_INTEGER_VALUE_STRUCT
{
    int64_t value;
} pesec_integer_value_t;

pesec_integer_value_t* pesec_integer_value_new(int64_t value);

void pesec_integer_value_destroy(pesec_integer_value_t* integer_value);

void pesec_integer_value_print(FILE* stream, const pesec_integer_value_t* integer_value);

int64_t pesec_integer_value_get(const pesec_integer_value_t* integer_value);


bool pesec_integer_value_is_truthy(const pesec_integer_value_t* integer_value);

pesec_value_t pesec_integer_value_negative(const pesec_integer_value_t* integer_value);

pesec_value_t pesec_integer_value_add(const pesec_integer_value_t* left, pesec_value_t right);

pesec_value_t pesec_integer_value_sub(const pesec_integer_value_t* left, pesec_value_t right);

pesec_value_t pesec_integer_value_mul(const pesec_integer_value_t* left, pesec_value_t right);

pesec_value_t pesec_integer_value_div(const pesec_integer_value_t* left, pesec_value_t right);

pesec_value_t pesec_integer_value_mod(const pesec_integer_value_t* left, pesec_value_t right);

bool pesec_integer_value_equal(const pesec_integer_value_t* left, pesec_value_t right);

bool pesec_integer_value_less(const pesec_integer_value_t* left, pesec_value_t right);

bool pesec_integer_value_less_equal(const pesec_integer_value_t* left, pesec_value_t right);

bool pesec_integer_value_greater(const pesec_integer_value_t* left, pesec_value_t right);

bool pesec_integer_value_greater_equal(const pesec_integer_value_t* left, pesec_value_t right);


pesec_value_t pesec_integer_value_method_handler(
    pesec_value_t* self,
    pesec_string_view_t field,
    pesec_environment_t* environment
);

#endif // PESEC_INTEGER_VALUE_H
