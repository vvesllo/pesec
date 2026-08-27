#include "../include/number_value.h"
#include <math.h>
#include <stdlib.h>
#include "include/utils/throw.h"


static void number_value_value_push_back(number_value_value_t* number_value_value, const int digit)
{
    if (number_value_value->length >= number_value_value->capacity)
    {
        number_value_value->capacity *= 2;
        const auto temp = (ull_t*)realloc(number_value_value->data, number_value_value->capacity * sizeof(ull_t));
        if (!temp) THROW("Failed to resize number value.");
        number_value_value->data = temp;
    }

    number_value_value->data[number_value_value->length++] = (ull_t)digit;
    number_value_value->size = number_value_value->length;
}

static number_value_value_t* number_value_value_new_empty()
{
    const auto number_value_value = (number_value_value_t*)malloc(sizeof(number_value_value_t));

    number_value_value->capacity = 1;
    number_value_value->size = 0;
    number_value_value->length = 0;
    number_value_value->data = (ull_t*)calloc(number_value_value->capacity, sizeof(ull_t));

    return number_value_value;
}

static number_value_value_t* number_value_value_new(const string_view_t string_view)
{
    const auto number_value_value = number_value_value_new_empty();

    for (ull_t i = 0; i < string_view.length; ++i)
        number_value_value_push_back(number_value_value, string_view.data[i] - '0');

    while (number_value_value->length > 1 && number_value_value->data[0] == 0)
    {
        for (size_t i = 0; i < number_value_value->length - 1; ++i) number_value_value->data[i] = number_value_value->data[i+1];
        number_value_value->length--;
    }

    return number_value_value;
}

number_value_t* number_value_new(const string_view_t decimal, const string_view_t fraction)
{
    const auto number_value = (number_value_t*)malloc(sizeof(number_value_t));
    number_value->negative = false;
    number_value->decimal = number_value_value_new(decimal);
    number_value->fraction = number_value_value_new(fraction);
    return number_value;
}

void number_value_free(number_value_t* number_value)
{
    free(number_value->decimal->data);
    free(number_value->decimal);
    free(number_value->fraction->data);
    free(number_value->fraction);
    free(number_value);
}

number_value_t* number_value_add(const number_value_t* right, const number_value_t* left)
{
    number_value_t* result = number_value_new(string_view_from(""), string_view_from(""));


    return result;
}

number_value_t* number_value_mul(const number_value_t* right, const number_value_t* left)
{
    number_value_t* result = number_value_new(string_view_from(""), string_view_from(""));


    return result;
}
