#ifndef PESEC_NUMBER_VALUE_H
#define PESEC_NUMBER_VALUE_H

#include "utils/typedefs.h"
#include "utils/string_view.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


#define NV_LIMB_DIGITS 9
#define NV_LIMB_BASE 1000000000u

#ifndef NV_MAX_LIMBS
#define NV_MAX_LIMBS ((u64_t)1 << 22)
#endif // NV_MAX_LIMBS

#define NUM_VAL_0 number_value_from_sv(string_view_from("0"))

typedef struct
{
    u32_t *limb;
    u64_t size;
    u64_t capacity;
} number_value_mantissa_t;

typedef struct NUMBER_VALUE_STRUCT
{
    bool negative;
    number_value_mantissa_t *mantissa;
    u64_t exponent;
} number_value_t;

void number_value_mantissa_set_limb(const number_value_mantissa_t *m, u64_t index, u32_t limb);

u32_t number_value_mantissa_get_limb(const number_value_mantissa_t *m, u64_t index);

void number_value_mantissa_push_limb(number_value_mantissa_t *m, u32_t limb);

u64_t number_value_mantissa_significant_size(const number_value_mantissa_t *m);

number_value_t *number_value_from_sv(string_view_t sv);

number_value_t *number_value_one();

number_value_t *number_value_zero();

number_value_t *number_value_new(number_value_mantissa_t *mantissa, u64_t exponent);

void number_value_free(number_value_t *v);

long double number_value_to_long_double(const number_value_t *v);

number_value_t *number_value_from_long_double(long double value);

char *number_value_to_string(const number_value_t *v);

number_value_t *number_value_negate(const number_value_t *v);

unsigned char number_value_aligned_digit_from_lsb(const number_value_t *v, u64_t aligned_exponent, u64_t index);

int number_value_compare_abs(const number_value_t *l, const number_value_t *r);

short number_value_compare(const number_value_t *l, const number_value_t *r);

number_value_t *number_value_add(const number_value_t *l, const number_value_t *r);

number_value_t *number_value_sub(const number_value_t *l, const number_value_t *r);

number_value_t *number_value_mul(const number_value_t *l, const number_value_t *r);

number_value_t *number_value_div(const number_value_t *l, const number_value_t *r);

number_value_t *number_value_floor_div(const number_value_t *l, const number_value_t *r);

number_value_t *number_value_pow(const number_value_t *l, const number_value_t *r);

#ifdef __cplusplus
}
#endif

#endif // PESEC_NUMBER_VALUE_H
