#include "../include/number_value.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "include/utils/interpret_info.h"
#include "include/utils/throw.h"


void number_value_mantissa_set_digit(const number_value_mantissa_t* number_value_mantissa, const ull_t index, unsigned char digit)
{
    const ull_t byte_index = index / 2;
    digit &= 0x0F;
    if (index % 2 == 0)
        number_value_mantissa->data[byte_index] = (number_value_mantissa->data[byte_index] & 0xF0) | digit;
    else
        number_value_mantissa->data[byte_index] = (number_value_mantissa->data[byte_index] & 0x0F) | (digit << 4);
}

void number_value_mantissa_push_digit(number_value_mantissa_t* number_value_mantissa, const unsigned char digit)
{
    if (number_value_mantissa->size >= number_value_mantissa->capacity * 2)
    {
        const ull_t old_capacity = number_value_mantissa->capacity;
        number_value_mantissa->capacity = old_capacity * 2;
        const auto temp = (unsigned char*)realloc(number_value_mantissa->data, number_value_mantissa->capacity * sizeof(unsigned char));
        if (!temp) THROW("Failed to realloc mantissa data");
        number_value_mantissa->data = temp;

        memset(number_value_mantissa->data + old_capacity, 0, (number_value_mantissa->capacity - old_capacity));
    }

    number_value_mantissa_set_digit(number_value_mantissa, number_value_mantissa->size, digit);
    ++number_value_mantissa->size;
}

ull_t number_value_mantissa_significant_size(const number_value_mantissa_t* number_value_mantissa)
{
    ull_t leading_zeros = 0;
    while (leading_zeros < number_value_mantissa->size && number_value_mantissa_get_digit(number_value_mantissa, leading_zeros) == 0)
        ++leading_zeros;
    return number_value_mantissa->size - leading_zeros;
}

unsigned char number_value_mantissa_get_digit(const number_value_mantissa_t* number_value_mantissa, const ull_t index)
{
    const ull_t byte_index = index / 2;

    if (index % 2 == 0)
        return number_value_mantissa->data[byte_index] & 0x0F;
    return (number_value_mantissa->data[byte_index] >> 4) & 0x0F;
}

static number_value_mantissa_t* number_value_mantissa_new(ull_t size)
{
    const auto number_value_mantissa = (number_value_mantissa_t*)malloc(sizeof(number_value_mantissa_t));
    if (!number_value_mantissa) THROW("Failed to alloc mantissa");

    number_value_mantissa->capacity = size / 2 + 1;
    number_value_mantissa->size = size;
    number_value_mantissa->data = (unsigned char*)calloc(number_value_mantissa->capacity, sizeof(unsigned char));
    if (!number_value_mantissa->data) THROW("Failed to alloc mantissa data");

    return number_value_mantissa;
}

static number_value_mantissa_t* number_value_mantissa_clone(const number_value_mantissa_t* source)
{
    number_value_mantissa_t* clone = number_value_mantissa_new(source->size);
    memcpy(clone->data, source->data, (source->size + 1) / 2);
    return clone;
}

static void number_value_mantissa_free(number_value_mantissa_t* number_value_mantissa)
{
    free(number_value_mantissa->data);
    free(number_value_mantissa);
}

static void number_value_mantissa_trim_leading_zeros(number_value_mantissa_t* mantissa)
{
    ull_t leading_zeros = 0;

    while (leading_zeros + 1 < mantissa->size && number_value_mantissa_get_digit(mantissa, leading_zeros) == 0)
        ++leading_zeros;

    if (leading_zeros == 0) return;

    const ull_t new_size = mantissa->size - leading_zeros;

    for (ull_t i = 0; i < new_size; ++i)
        number_value_mantissa_set_digit(mantissa, i, number_value_mantissa_get_digit(mantissa, leading_zeros + i));

    mantissa->size = new_size;
}

static void number_value_mantissa_push_zeros(number_value_mantissa_t* mantissa, const ull_t count)
{
    for (ull_t i = 0; i < count; ++i)
    {
        number_value_mantissa_push_digit(mantissa, 0);
    }
}

static int number_value_mantissa_compare_abs(const number_value_mantissa_t* left, const number_value_mantissa_t* right)
{
    const ull_t left_size = number_value_mantissa_significant_size(left);
    const ull_t right_size = number_value_mantissa_significant_size(right);

    if (left_size != right_size) return left_size < right_size ? -1 : 1;

    const ull_t left_offset = left->size - left_size;
    const ull_t right_offset = right->size - right_size;

    for (ull_t i = 0; i < left_size; ++i)
    {
        const unsigned char left_digit = number_value_mantissa_get_digit(left, left_offset + i);
        const unsigned char right_digit = number_value_mantissa_get_digit(right, right_offset + i);
        if (left_digit != right_digit) return left_digit < right_digit ? -1 : 1;
    }

    return 0;
}

static unsigned char number_value_mantissa_digit_from_lsb(const number_value_mantissa_t* mantissa, const ull_t index)
{
    if (index >= mantissa->size) return 0;
    return number_value_mantissa_get_digit(mantissa, mantissa->size - 1 - index);
}

static number_value_mantissa_t* number_value_mantissa_add_abs(
    const number_value_mantissa_t* left_mantissa,
    const number_value_mantissa_t* right_mantissa)
{
    const ull_t size = left_mantissa->size > right_mantissa->size ? left_mantissa->size : right_mantissa->size;
    number_value_mantissa_t* result = number_value_mantissa_new(size + 1);
    unsigned char carry = 0;

    for (ull_t i = 0; i < size; ++i)
    {
        const unsigned char sum = number_value_mantissa_digit_from_lsb(left_mantissa, i)
                                + number_value_mantissa_digit_from_lsb(right_mantissa, i)
                                + carry;
        number_value_mantissa_set_digit(result, size - i, sum % 10);
        carry = sum / 10;
    }

    number_value_mantissa_set_digit(result, 0, carry);

    return result;
}

static void number_value_mantissa_sub_abs_in_place(
    number_value_mantissa_t* left_mantissa,
    const number_value_mantissa_t* right_mantissa)
{
    unsigned char borrow = 0;

    for (ull_t i = 0; i < left_mantissa->size; ++i)
    {
        int diff = (int)number_value_mantissa_digit_from_lsb(left_mantissa, i)
                 - (int)number_value_mantissa_digit_from_lsb(right_mantissa, i)
                 - borrow;

        if (diff < 0)
        {
            diff += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        number_value_mantissa_set_digit(left_mantissa, left_mantissa->size - 1 - i, (unsigned char)diff);
    }
}

static void number_value_mantissa_div_mod_digit(
    number_value_mantissa_t* remainder,
    const number_value_mantissa_t* divisor,
    const number_value_mantissa_t* double_divisor,
    const number_value_mantissa_t* quadruple_divisor,
    const number_value_mantissa_t* octuple_divisor,
    unsigned char* quotient_digit)
{
    unsigned char digit = 0;

    if (number_value_mantissa_compare_abs(remainder, octuple_divisor) >= 0)
    {
        number_value_mantissa_sub_abs_in_place(remainder, octuple_divisor);
        digit += 8;
    }
    if (number_value_mantissa_compare_abs(remainder, quadruple_divisor) >= 0)
    {
        number_value_mantissa_sub_abs_in_place(remainder, quadruple_divisor);
        digit += 4;
    }
    if (number_value_mantissa_compare_abs(remainder, double_divisor) >= 0)
    {
        number_value_mantissa_sub_abs_in_place(remainder, double_divisor);
        digit += 2;
    }
    if (number_value_mantissa_compare_abs(remainder, divisor) >= 0)
    {
        number_value_mantissa_sub_abs_in_place(remainder, divisor);
        digit += 1;
    }

    number_value_mantissa_trim_leading_zeros(remainder);

    *quotient_digit = digit;
}

static unsigned char number_value_mantissa_shifted_digit_from_lsb(
    const number_value_mantissa_t* mantissa,
    const ull_t shift,
    const ull_t index)
{
    if (index < shift) return 0;
    return number_value_mantissa_digit_from_lsb(mantissa, index - shift);
}

static number_value_mantissa_t* number_value_mantissa_add_shifted_abs(
    const number_value_mantissa_t* left_mantissa, const ull_t left_shift,
    const number_value_mantissa_t* right_mantissa, const ull_t right_shift)
{
    const ull_t left_size = left_mantissa->size + left_shift;
    const ull_t right_size = right_mantissa->size + right_shift;
    const ull_t size = left_size > right_size ? left_size : right_size;

    number_value_mantissa_t* result = number_value_mantissa_new(size + 1);
    unsigned char carry = 0;

    for (ull_t i = 0; i < size; ++i)
    {
        const unsigned char sum = number_value_mantissa_shifted_digit_from_lsb(left_mantissa, left_shift, i)
                                + number_value_mantissa_shifted_digit_from_lsb(right_mantissa, right_shift, i)
                                + carry;
        number_value_mantissa_set_digit(result, size - i, sum % 10);
        carry = sum / 10;
    }

    number_value_mantissa_set_digit(result, 0, carry);

    return result;
}

static number_value_mantissa_t* number_value_mantissa_sub_shifted_abs(
    const number_value_mantissa_t* left_mantissa, const ull_t left_shift,
    const number_value_mantissa_t* right_mantissa, const ull_t right_shift)
{
    const ull_t left_size = left_mantissa->size + left_shift;
    const ull_t right_size = right_mantissa->size + right_shift;
    const ull_t size = left_size > right_size ? left_size : right_size;

    number_value_mantissa_t* result = number_value_mantissa_new(size);
    unsigned char borrow = 0;

    for (ull_t i = 0; i < size; ++i)
    {
        int diff = (int)number_value_mantissa_shifted_digit_from_lsb(left_mantissa, left_shift, i)
                 - (int)number_value_mantissa_shifted_digit_from_lsb(right_mantissa, right_shift, i)
                 - borrow;

        if (diff < 0)
        {
            diff += 10;
            borrow = 1;
        }
        else
            borrow = 0;

        number_value_mantissa_set_digit(result, size - 1 - i, (unsigned char)diff);
    }

    return result;
}

static int number_value_mantissa_compare_shifted_abs(
    const number_value_mantissa_t* left_mantissa, const ull_t left_shift,
    const number_value_mantissa_t* right_mantissa, const ull_t right_shift)
{
    const ull_t left_size = left_mantissa->size + left_shift;
    const ull_t right_size = right_mantissa->size + right_shift;
    const ull_t size = left_size > right_size ? left_size : right_size;

    for (ull_t i = size; i-- > 0; )
    {
        const unsigned char left_digit = number_value_mantissa_shifted_digit_from_lsb(left_mantissa, left_shift, i);
        const unsigned char right_digit = number_value_mantissa_shifted_digit_from_lsb(right_mantissa, right_shift, i);
        if (left_digit != right_digit) return left_digit < right_digit ? -1 : 1;
    }

    return 0;
}

static void number_value_normalize(number_value_t* number_value)
{
    const ull_t accuracy = interpret_info_get()->number_accuracy;

    if (number_value->exponent > accuracy)
    {
        const ull_t excess = number_value->exponent - accuracy;

        if (excess >= number_value->mantissa->size)
        {
            number_value->mantissa->size = 1;
            number_value_mantissa_set_digit(number_value->mantissa, 0, 0);
            number_value->exponent = 0;
        }
        else
        {
            number_value->mantissa->size -= excess;
            number_value->exponent = accuracy;
        }
    }

    number_value_mantissa_trim_leading_zeros(number_value->mantissa);

    while (number_value->exponent > 0 && number_value->mantissa->size > 1 &&
           number_value_mantissa_get_digit(number_value->mantissa, number_value->mantissa->size - 1) == 0)
    {
        --number_value->mantissa->size;
        --number_value->exponent;
    }

    if (number_value_mantissa_significant_size(number_value->mantissa) == 0)
    {
        number_value->mantissa->size = 1;
        number_value_mantissa_set_digit(number_value->mantissa, 0, 0);
        number_value->negative = false;
        number_value->exponent = 0;
    }
}

static number_value_t* number_value_clone(const number_value_t* source)
{
    number_value_t* clone = number_value_new(number_value_mantissa_clone(source->mantissa), source->exponent);
    clone->negative = source->negative;
    return clone;
}

static bool number_value_is_integer(const number_value_t* number_value)
{
    const number_value_mantissa_t* mantissa = number_value->mantissa;

    if (number_value_mantissa_significant_size(mantissa) == 0) return true;
    if (mantissa->size < number_value->exponent) return false;

    for (ull_t i = mantissa->size - number_value->exponent; i < mantissa->size; ++i)
        if (number_value_mantissa_get_digit(mantissa, i) != 0) return false;

    return true;
}

number_value_t* number_value_from_sv(const string_view_t string_view)
{
    const auto mantissa = (number_value_mantissa_t*)malloc(sizeof(number_value_mantissa_t));
    if (!mantissa) THROW("Failed to alloc mantissa");

    mantissa->capacity = 16;
    mantissa->size = 0;
    mantissa->data = (unsigned char*)calloc(mantissa->capacity, sizeof(unsigned char));
    if (!mantissa->data) THROW("Failed to alloc mantissa data");

    const ull_t accuracy = interpret_info_get()->number_accuracy;

    bool floating = false;
    ull_t exponent = 0;

    for (ull_t i = 0; i < string_view.length; i++)
    {
        const char current_char = string_view.data[i];
        if (current_char == '.')
        {
            floating = true;
            continue;
        }

        if (current_char < '0' || current_char > '9')
            THROW("Invalid character in number literal");

        number_value_mantissa_push_digit(mantissa, (unsigned char)(current_char - '0'));
        if (floating) ++exponent;
        if (exponent >= accuracy) break;
    }

    return number_value_new(mantissa, exponent);
}

number_value_t* number_value_one()
{
    number_value_mantissa_t* mantissa = number_value_mantissa_new(1);
    number_value_mantissa_set_digit(mantissa, 0, 1);
    return number_value_new(mantissa, 0);
}

number_value_t* number_value_zero()
{
    number_value_mantissa_t* mantissa = number_value_mantissa_new(1);
    number_value_mantissa_set_digit(mantissa, 0, 0);
    return number_value_new(mantissa, 0);
}

number_value_t* number_value_new(number_value_mantissa_t* mantissa, const ull_t exponent)
{
    const auto number_value = (number_value_t*)malloc(sizeof(number_value_t));
    if (!number_value) THROW("Failed to alloc number value");

    number_value->negative = false;
    number_value->exponent = exponent;
    number_value->mantissa = mantissa;

    return number_value;
}

void number_value_free(number_value_t* number_value)
{
    number_value_mantissa_free(number_value->mantissa);
    free(number_value);
}

long double number_value_to_long_double(const number_value_t* number_value)
{
    long double result = 0.0L;

    for (ull_t i = 0; i < number_value->mantissa->size; ++i)
        result = result * 10.0L + number_value_mantissa_get_digit(number_value->mantissa, i);

    for (ull_t i = 0; i < number_value->exponent; ++i)
        result /= 10.0L;

    return number_value->negative ? -result : result;
}

number_value_t* number_value_from_long_double(long double value)
{
    if (!isfinite(value)) THROW("Cannot convert a non-finite value");

    const auto mantissa = (number_value_mantissa_t*)malloc(sizeof(number_value_mantissa_t));
    if (!mantissa) THROW("Failed to alloc mantissa");

    mantissa->capacity = 16;
    mantissa->size = 0;
    mantissa->data = (unsigned char*)calloc(mantissa->capacity, sizeof(unsigned char));
    if (!mantissa->data) THROW("Failed to alloc mantissa data");

    const bool negative = value < 0.0L;
    if (negative) value = -value;

    const ull_t accuracy = interpret_info_get()->number_accuracy;
    ull_t exponent = 0;

    long double integer_part = floorl(value);
    long double fractional_part = value - integer_part;

    if (integer_part > 0.0L)
    {
        long double power = 1.0L;
        while (integer_part / power >= 10.0L)
            power *= 10.0L;

        while (power >= 1.0L)
        {
            unsigned char digit = (unsigned char)floorl(integer_part / power);
            if (digit > 9) digit = 9;
            number_value_mantissa_push_digit(mantissa, digit);
            integer_part = fmodl(integer_part, power);
            power /= 10.0L;
        }
    }

    while (exponent < accuracy && fractional_part > 0.0L)
    {
        fractional_part *= 10.0L;
        unsigned char digit = (unsigned char)fractional_part;
        if (digit > 9) digit = 9;
        number_value_mantissa_push_digit(mantissa, digit);
        fractional_part -= (long double)digit;
        ++exponent;
    }

    if (mantissa->size == 0)
        number_value_mantissa_push_digit(mantissa, 0);

    number_value_t* result = number_value_new(mantissa, exponent);
    result->negative = negative;

    number_value_normalize(result);
    return result;
}

number_value_t* number_value_negate(const number_value_t* number_value)
{
    number_value_t* negative = number_value_new(
        number_value_mantissa_clone(number_value->mantissa),
        number_value->exponent
    );
    negative->negative = !number_value->negative;

    number_value_normalize(negative);
    return negative;
}

unsigned char number_value_aligned_digit_from_lsb(
    const number_value_t* number_value,
    const ull_t aligned_exponent,
    const ull_t index)
{
    return number_value_mantissa_shifted_digit_from_lsb(
        number_value->mantissa,
        aligned_exponent - number_value->exponent,
        index
    );
}

int number_value_compare_abs(const number_value_t* left, const number_value_t* right)
{
    const ull_t aligned_exponent = left->exponent > right->exponent ? left->exponent : right->exponent;

    return number_value_mantissa_compare_shifted_abs(
        left->mantissa, aligned_exponent - left->exponent,
        right->mantissa, aligned_exponent - right->exponent
    );
}

short number_value_compare(const number_value_t* left, const number_value_t* right)
{
    const bool left_zero = number_value_mantissa_significant_size(left->mantissa) == 0;
    const bool right_zero = number_value_mantissa_significant_size(right->mantissa) == 0;

    if (left_zero && right_zero) return 0;
    if (left_zero) return right->negative ? 1 : -1;
    if (right_zero) return left->negative ? -1 : 1;

    if (left->negative != right->negative)
        return left->negative ? -1 : 1;

    const int magnitude_compare = number_value_compare_abs(left, right);
    return (short)(left->negative ? -magnitude_compare : magnitude_compare);
}

number_value_t* number_value_add(const number_value_t* left, const number_value_t* right)
{
    const ull_t max_exponent = left->exponent > right->exponent ? left->exponent : right->exponent;
    const ull_t left_shift = max_exponent - left->exponent;
    const ull_t right_shift = max_exponent - right->exponent;

    number_value_t* result = number_value_new(nullptr, max_exponent);

    if (left->negative == right->negative)
    {
        result->negative = left->negative;
        result->mantissa = number_value_mantissa_add_shifted_abs(
            left->mantissa, left_shift,
            right->mantissa, right_shift
        );
    }
    else
    {
        const int abs_compare = number_value_mantissa_compare_shifted_abs(
            left->mantissa, left_shift,
            right->mantissa, right_shift
        );

        if (abs_compare == 0)
        {
            result->negative = false;
            result->mantissa = number_value_mantissa_new(1);
        }
        else if (abs_compare > 0)
        {
            result->negative = left->negative;
            result->mantissa = number_value_mantissa_sub_shifted_abs(
                left->mantissa, left_shift,
                right->mantissa, right_shift
            );
        }
        else
        {
            result->negative = right->negative;
            result->mantissa = number_value_mantissa_sub_shifted_abs(
                right->mantissa, right_shift,
                left->mantissa, left_shift
            );
        }
    }

    number_value_normalize(result);
    return result;
}

number_value_t* number_value_sub(const number_value_t* left, const number_value_t* right)
{
    number_value_t* negated_right = number_value_negate(right);
    number_value_t* result = number_value_add(left, negated_right);
    number_value_free(negated_right);
    return result;
}

static number_value_t* number_value_decremented(const number_value_t* value)
{
    number_value_t* one = number_value_one();
    number_value_t* result = number_value_sub(value, one);
    number_value_free(one);
    return result;
}

number_value_t* number_value_mul(const number_value_t* left, const number_value_t* right)
{
    const number_value_mantissa_t* left_mantissa = left->mantissa;
    const number_value_mantissa_t* right_mantissa = right->mantissa;

    const ull_t size = left_mantissa->size + right_mantissa->size;

    const auto buffer = (ull_t*)calloc(size, sizeof(ull_t));
    if (!buffer) THROW("Failed to alloc multiplication buffer");

    for (ull_t i = 0; i < left_mantissa->size; ++i)
    {
        const ull_t left_digit = number_value_mantissa_digit_from_lsb(left_mantissa, i);
        if (left_digit == 0) continue;
        for (ull_t j = 0; j < right_mantissa->size; ++j)
            buffer[i + j] += left_digit * number_value_mantissa_digit_from_lsb(right_mantissa, j);
    }

    number_value_mantissa_t* mantissa = number_value_mantissa_new(size);

    ull_t carry = 0;
    for (ull_t i = 0; i < size; ++i)
    {
        const ull_t current = buffer[i] + carry;
        number_value_mantissa_set_digit(mantissa, size - 1 - i, (unsigned char)(current % 10));
        carry = current / 10;
    }
    free(buffer);

    number_value_t* result = number_value_new(mantissa, left->exponent + right->exponent);
    result->negative = left->negative != right->negative;

    number_value_normalize(result);
    return result;
}

number_value_t* number_value_div(const number_value_t* left, const number_value_t* right)
{
    if (number_value_mantissa_significant_size(right->mantissa) == 0)
        THROW("Division by zero");

    const ull_t accuracy = interpret_info_get()->number_accuracy;

    number_value_mantissa_t* dividend = number_value_mantissa_clone(left->mantissa);
    const number_value_mantissa_t* divisor = right->mantissa;

    number_value_mantissa_push_zeros(dividend, left->exponent > right->exponent ? 0 : right->exponent - left->exponent);

    number_value_mantissa_t* double_divisor = number_value_mantissa_add_abs(divisor, divisor);
    number_value_mantissa_t* quadruple_divisor = number_value_mantissa_add_abs(double_divisor, double_divisor);
    number_value_mantissa_t* octuple_divisor = number_value_mantissa_add_abs(quadruple_divisor, quadruple_divisor);

    number_value_mantissa_t* quotient = number_value_mantissa_new(0);
    number_value_mantissa_t* remainder = number_value_mantissa_new(0);

    for (ull_t i = 0; i < dividend->size; ++i)
    {
        number_value_mantissa_push_digit(remainder, number_value_mantissa_get_digit(dividend, i));
        unsigned char digit = 0;
        number_value_mantissa_div_mod_digit(remainder, divisor, double_divisor, quadruple_divisor, octuple_divisor, &digit);
        number_value_mantissa_push_digit(quotient, digit);
    }

    ull_t exponent = left->exponent > right->exponent ? left->exponent - right->exponent : 0;

    while (exponent < accuracy && number_value_mantissa_significant_size(remainder) != 0)
    {
        number_value_mantissa_push_digit(remainder, 0);
        unsigned char digit = 0;
        number_value_mantissa_div_mod_digit(remainder, divisor, double_divisor, quadruple_divisor, octuple_divisor, &digit);
        number_value_mantissa_push_digit(quotient, digit);
        ++exponent;
    }

    number_value_mantissa_free(dividend);
    number_value_mantissa_free(remainder);
    number_value_mantissa_free(double_divisor);
    number_value_mantissa_free(quadruple_divisor);
    number_value_mantissa_free(octuple_divisor);

    number_value_t* result = number_value_new(quotient, exponent);
    result->negative = left->negative != right->negative;

    number_value_normalize(result);
    return result;
}

number_value_t* number_value_floor_div(const number_value_t* left, const number_value_t* right)
{
    number_value_t* quotient = number_value_div(left, right);

    number_value_mantissa_t* mantissa = quotient->mantissa;
    bool has_fraction = false;

    if (mantissa->size > quotient->exponent)
    {
        for (ull_t i = mantissa->size - quotient->exponent; i < mantissa->size; ++i)
        {
            if (number_value_mantissa_get_digit(mantissa, i) != 0)
            {
                has_fraction = true;
                break;
            }
        }
        mantissa->size -= quotient->exponent;
    }
    else
    {
        has_fraction = number_value_mantissa_significant_size(mantissa) != 0;
        mantissa->size = 1;
        number_value_mantissa_set_digit(mantissa, 0, 0);
    }
    quotient->exponent = 0;
    number_value_mantissa_trim_leading_zeros(mantissa);

    if (!quotient->negative) return quotient;

    if (has_fraction)
    {
        number_value_t* decremented = number_value_decremented(quotient);
        number_value_free(quotient);
        return decremented;
    }

    number_value_t* product = number_value_mul(quotient, right);
    number_value_t* remainder = number_value_sub(left, product);
    number_value_free(product);

    const bool exact = number_value_mantissa_significant_size(remainder->mantissa) == 0;
    number_value_free(remainder);

    if (exact) return quotient;

    number_value_t* decremented = number_value_decremented(quotient);
    number_value_free(quotient);
    return decremented;
}

number_value_t* number_value_pow(const number_value_t* left, const number_value_t* right)
{
    if (right->negative && number_value_mantissa_significant_size(left->mantissa) == 0)
        THROW("Zero raised to a negative power");

    if (!number_value_is_integer(right))
        return number_value_from_long_double(
            powl(number_value_to_long_double(left), number_value_to_long_double(right))
        );

    const ull_t integer_digits = right->mantissa->size > right->exponent
        ? right->mantissa->size - right->exponent
        : 0;

    ull_t exponent = 0;
    for (ull_t i = 0; i < integer_digits; ++i)
    {
        const unsigned char digit = number_value_mantissa_get_digit(right->mantissa, i);
        if (exponent > (ULLONG_MAX - digit) / 10)
            THROW("Exponent is too large");
        exponent = exponent * 10 + digit;
    }

    number_value_t* result = number_value_one();
    number_value_t* base = number_value_clone(left);

    while (exponent > 0)
    {
        if (exponent & 1)
        {
            number_value_t* temp = number_value_mul(result, base);
            number_value_free(result);
            result = temp;
        }
        exponent >>= 1;
        if (exponent > 0)
        {
            number_value_t* temp = number_value_mul(base, base);
            number_value_free(base);
            base = temp;
        }
    }
    number_value_free(base);

    if (right->negative)
    {
        number_value_t* one = number_value_one();
        number_value_t* reciprocal = number_value_div(one, result);
        number_value_free(one);
        number_value_free(result);
        result = reciprocal;
    }

    return result;
}