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

number_value_t* number_value_add(const number_value_t* right, const number_value_t* left);

number_value_t* number_value_mul(const number_value_t* right, const number_value_t* left);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PESEC_NUMBER_VALUE_H