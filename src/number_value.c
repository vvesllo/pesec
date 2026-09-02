#include "../include/number_value.h"

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

void number_value_mantissa_push_digit(number_value_mantissa_t* number_value_mantissa, unsigned char digit)
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

unsigned char number_value_mantissa_get_digit(const number_value_mantissa_t* number_value_mantissa, const ull_t index)
{
    const ull_t byte_index = index / 2;

    if (index % 2 == 0)
        return number_value_mantissa->data[byte_index] & 0x0F;
    return (number_value_mantissa->data[byte_index] >> 4) & 0x0F;
}

number_value_t* number_value_new(const string_view_t number_string_view)
{
    const auto number_value = (number_value_t*)malloc(sizeof(number_value_t));

    number_value->negative = false;
    number_value->exponent = 0;
    number_value->mantissa = (number_value_mantissa_t*)malloc(sizeof(number_value_mantissa_t));
    number_value->mantissa->capacity = 16;
    number_value->mantissa->size = 0;
    number_value->mantissa->data = (unsigned char*)calloc(number_value->mantissa->capacity, sizeof(unsigned char));

    bool floating = false;

    for (ull_t i = 0; i < number_string_view.length; i++)
    {
        const char current_char = number_string_view.data[i];
        if (current_char == '.')
        {
            floating = true;
            continue;
        }

        const int digit = current_char - '0';
        number_value_mantissa_push_digit(number_value->mantissa, digit);
        if (floating) ++number_value->exponent;
        if (number_value->exponent >= interpret_info_get()->number_accuracy) break;
    }

    return number_value;
}

void number_value_free(number_value_t* number_value)
{
    free(number_value->mantissa->data);
    free(number_value->mantissa);
    free(number_value);
}

long double number_value_to_long_double(number_value_t* number_value)
{

}

number_value_t *number_value_from_long_double(long double value)
{

}

number_value_t* number_value_negate(const number_value_t* number_value)
{

}

short number_value_compare(const number_value_t* left, const number_value_t* right)
{

}

number_value_t* number_value_add(const number_value_t* left, const number_value_t* right)
{

}

number_value_t* number_value_sub(const number_value_t* left, const number_value_t* right)
{

}

number_value_t* number_value_mul(const number_value_t* left, const number_value_t* right)
{

}

number_value_t* number_value_div(const number_value_t* left, const number_value_t* right)
{

}

number_value_t* number_value_floor_div(const number_value_t* left, const number_value_t* right)
{

}

number_value_t* number_value_pow(const number_value_t* left, const number_value_t* right)
{

}
