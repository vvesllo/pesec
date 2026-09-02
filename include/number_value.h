#ifndef PESEC_NUMBER_VALUE_H
#define PESEC_NUMBER_VALUE_H

#include "utils/typedefs.h"
#include "utils/string_view.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define NUM_VAL_0 number_value_new(string_view_from("0"))

typedef struct
{
    ull_t size;
    ull_t capacity;
    unsigned char* data;
} number_value_mantissa_t;

typedef struct NUMBER_VALUE_STRUCT
{
    bool negative;
    number_value_mantissa_t* mantissa;
    ull_t exponent;
} number_value_t;

void number_value_mantissa_set_digit(const number_value_mantissa_t* number_value_mantissa, const ull_t index, unsigned char digit);

void number_value_mantissa_push_digit(number_value_mantissa_t* number_value_mantissa, unsigned char digit);

unsigned char number_value_mantissa_get_digit(const number_value_mantissa_t* number_value_mantissa, const ull_t index);

number_value_t* number_value_new(string_view_t number_string_view);

void number_value_free(number_value_t* number_value);

long double number_value_to_long_double(number_value_t* number_value);

number_value_t *number_value_from_long_double(long double value);

number_value_t* number_value_negate(const number_value_t* number_value);

short number_value_compare(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_add(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_sub(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_mul(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_div(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_floor_div(const number_value_t* left, const number_value_t* right);

number_value_t* number_value_pow(const number_value_t* left, const number_value_t* right);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // PESEC_NUMBER_VALUE_H