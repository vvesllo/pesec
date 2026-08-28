#ifndef PESEC_NUMBER_VALUE_H
#define PESEC_NUMBER_VALUE_H

#include "utils/typedefs.h"
#include "utils/string_view.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct
{
    ull_t size;
    ull_t capacity;
    ull_t length;
    ull_t* data;
} number_value_value_t;

typedef struct
{
    bool negative;
    number_value_value_t* decimal;
    number_value_value_t* fraction;
} number_value_t;

number_value_t* number_value_new(string_view_t decimal, string_view_t fraction);

void number_value_free(number_value_t* number_value);

int number_value_compare_abs(const number_value_t* left, const number_value_t* right);

int number_value_compare(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_add_abs(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_sub_abs(const number_value_t* left, const number_value_t* right);

ull_t* multiply_integer(const ull_t* left, ull_t left_length, const ull_t* right, ull_t right_length, ull_t* out_length);

number_value_t* number_value_add(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_sub(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_mul(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_div(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_negate(const number_value_t* number_value);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // PESEC_NUMBER_VALUE_H