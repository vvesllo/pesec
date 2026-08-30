#include "../include/number_value.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/utils/interpret_info.h"
#include "include/utils/throw.h"

static number_value_value_t *number_value_value_new_empty()
{
    const auto number_value_value = (number_value_value_t *) malloc(sizeof(number_value_value_t));
    number_value_value->capacity = 4;
    number_value_value->size = 0;
    number_value_value->length = 0;
    number_value_value->data = (ull_t *) calloc(number_value_value->capacity, sizeof(ull_t));
    return number_value_value;
}

static void number_value_value_push_back(number_value_value_t *number_value_value, const ull_t digit)
{
    if (number_value_value->length >= number_value_value->capacity)
    {
        number_value_value->capacity *= 2;
        const auto temp = (ull_t*)realloc(number_value_value->data, number_value_value->capacity * sizeof(ull_t));
        if (!temp) THROW("Failed to resize number value.");
        number_value_value->data = temp;
    }

    number_value_value->data[number_value_value->length++] = digit;
    number_value_value->size = number_value_value->length;
}

static number_value_value_t *number_value_value_new_from_string(const string_view_t sv)
{
    number_value_value_t *number_value_value = number_value_value_new_empty();
    for (ull_t i = 0; i < sv.length; ++i)
    {
        if (sv.data[i] >= '0' && sv.data[i] <= '9')
            number_value_value_push_back(number_value_value, sv.data[i] - '0');
    }
    return number_value_value;
}

static number_value_value_t *number_value_value_clone(const number_value_value_t *src)
{
    if (!src) return nullptr;
    const auto clone = (number_value_value_t *) malloc(sizeof(number_value_value_t));
    clone->capacity = src->capacity;
    clone->size = src->size;
    clone->length = src->length;
    clone->data = (ull_t *) malloc(clone->capacity * sizeof(ull_t));
    memcpy(clone->data, src->data, src->length * sizeof(ull_t));
    return clone;
}

static void number_value_value_free(number_value_value_t *number_value_value)
{
    if (number_value_value)
    {
        free(number_value_value->data);
        free(number_value_value);
    }
}

static void number_value_value_trim_lead(number_value_value_t *number_value_value)
{
    if (!number_value_value || number_value_value->length == 0) return;
    ull_t i = 0;
    while (i < number_value_value->length - 1 && number_value_value->data[i] == 0) i++;
    if (i > 0)
    {
        memmove(number_value_value->data, number_value_value->data + i, (number_value_value->length - i) * sizeof(ull_t));
        number_value_value->length -= i;
        number_value_value->size = number_value_value->length;
    }
}

static void number_value_value_trim_trail(number_value_value_t *number_value_value)
{
    if (!number_value_value || number_value_value->length == 0) return;
    while (number_value_value->length > 0 && number_value_value->data[number_value_value->length - 1] == 0)
        number_value_value->length--;
    number_value_value->size = number_value_value->length;
}

number_value_t *number_value_new(const string_view_t decimal, const string_view_t fraction)
{
    const auto number_value = (number_value_t *) malloc(sizeof(number_value_t));
    number_value->negative = false;
    number_value->decimal = number_value_value_new_from_string(decimal);
    if (number_value->decimal->length == 0)
        number_value_value_push_back(number_value->decimal, 0);
    number_value->fraction = number_value_value_new_from_string(fraction);
    return number_value;
}

void number_value_free(number_value_t *number_value)
{
    if (!number_value) return;
    number_value_value_free(number_value->decimal);
    number_value_value_free(number_value->fraction);
    free(number_value);
}

long double number_value_to_long_double(number_value_t* number_value)
{
    if (!number_value) return 0.0L;

    long double result = 0.0L;
    for (ull_t i = 0; i < number_value->decimal->length; ++i)
        result = result * 10.0L + (long double)number_value->decimal->data[i];

    long double frac = 0.0L;
    for (ull_t i = 0; i < number_value->fraction->length; ++i)
        frac = frac * 10.0L + (long double)number_value->fraction->data[i];

    if (number_value->fraction->length > 0)
    {
        long double divisor = 1.0L;
        for (ull_t i = 0; i < number_value->fraction->length; ++i)
            divisor *= 10.0L;

        frac /= divisor;
    }

    result += frac;

    if (number_value->negative) result = -result;

    return result;
}

void number_value_normalize(number_value_t *number_value)
{
    if (!number_value) return;
    number_value_value_trim_lead(number_value->decimal);
    if (number_value->decimal->length == 0)
        number_value_value_push_back(number_value->decimal, 0);
    number_value_value_trim_trail(number_value->fraction);
    if (number_value->decimal->length == 1 && number_value->decimal->data[0] == 0 && number_value->fraction->length == 0)
        number_value->negative = false;
}

int number_value_compare_abs(const number_value_t *left, const number_value_t *right)
{
    if (left->decimal->length != right->decimal->length)
        return (left->decimal->length > right->decimal->length) ? 1 : -1;
    for (ull_t i = 0; i < left->decimal->length; i++)
    {
        if (left->decimal->data[i] != right->decimal->data[i])
            return (left->decimal->data[i] > right->decimal->data[i]) ? 1 : -1;
    }

    const ull_t max_frac = (left->fraction->length > right->fraction->length) ? left->fraction->length : right->fraction->length;
    for (ull_t i = 0; i < max_frac; i++)
    {
        const ull_t da = (i < left->fraction->length) ? left->fraction->data[i] : 0;
        const ull_t db = (i < right->fraction->length) ? right->fraction->data[i] : 0;
        if (da != db)
            return (da > db) ? 1 : -1;
    }
    return 0;
}

int number_value_compare(const number_value_t *left, const number_value_t *right)
{
    if (left->negative != right->negative)
        return left->negative ? -1 : 1;

    const int compare = number_value_compare_abs(left, right);
    return left->negative ? -compare : compare;
}


number_value_t *number_value_add_abs(const number_value_t *left, const number_value_t *right)
{
    const auto result = (number_value_t *) malloc(sizeof(number_value_t));
    result->negative = false;
    const ull_t max_frac = (left->fraction->length > right->fraction->length) ? left->fraction->length : right->fraction->length;
    const auto frac_res = (ull_t *) calloc(max_frac, sizeof(ull_t));
    ull_t carry = 0;


    for (ull_t i = max_frac; i > 0; i--)
    {
        ull_t pos = i - 1;
        ull_t da = (pos < left->fraction->length) ? left->fraction->data[pos] : 0;
        ull_t db = (pos < right->fraction->length) ? right->fraction->data[pos] : 0;
        ull_t sum = da + db + carry;
        frac_res[pos] = sum % 10;
        carry = sum / 10;
    }

    result->fraction = number_value_value_new_empty();
    for (ull_t i = 0; i < max_frac; i++)
        number_value_value_push_back(result->fraction, frac_res[i]);
    free(frac_res);


    const ull_t max_int = (left->decimal->length > right->decimal->length) ? left->decimal->length : right->decimal->length;
    const auto int_res = (ull_t *) calloc(max_int + 1, sizeof(ull_t));

    for (ull_t i = 0; i < max_int; i++)
    {
        ull_t da = (i < left->decimal->length) ? left->decimal->data[left->decimal->length - 1 - i] : 0;
        ull_t db = (i < right->decimal->length) ? right->decimal->data[right->decimal->length - 1 - i] : 0;
        ull_t sum = da + db + carry;
        int_res[max_int - i] = sum % 10;
        carry = sum / 10;
    }
    int_res[0] = carry;

    result->decimal = number_value_value_new_empty();
    const ull_t start = (int_res[0] > 0) ? 0 : 1;
    for (ull_t i = start; i <= max_int; i++)
        number_value_value_push_back(result->decimal, int_res[i]);
    free(int_res);
    number_value_normalize(result);
    return result;
}


number_value_t *number_value_sub_abs(const number_value_t *left, const number_value_t *right)
{
    const auto result = (number_value_t *) malloc(sizeof(number_value_t));
    result->negative = false;
    const ull_t max_frac = (left->fraction->length > right->fraction->length) ? left->fraction->length : right->fraction->length;
    const auto frac_res = (ull_t *) calloc(max_frac, sizeof(ull_t));
    ull_t borrow = 0;

    for (ull_t i = max_frac; i > 0; i--)
    {
        ull_t pos = i - 1;
        ull_t da = (pos < left->fraction->length) ? left->fraction->data[pos] : 0;
        ull_t db = (pos < right->fraction->length) ? right->fraction->data[pos] : 0;
        db += borrow;
        if (da < db)
        {
            da += 10;
            borrow = 1;
        } else
            borrow = 0;
        frac_res[pos] = da - db;
    }

    result->fraction = number_value_value_new_empty();
    for (ull_t i = 0; i < max_frac; i++)
        number_value_value_push_back(result->fraction, frac_res[i]);
    free(frac_res);


    const ull_t max_int = left->decimal->length;
    const auto int_res = (ull_t *) calloc(max_int, sizeof(ull_t));
    for (ull_t i = 0; i < max_int; i++)
    {
        ull_t da = left->decimal->data[left->decimal->length - 1 - i];
        ull_t db = (i < right->decimal->length) ? right->decimal->data[right->decimal->length - 1 - i] : 0;
        db += borrow;
        if (da < db)
        {
            da += 10;
            borrow = 1;
        }
        else
            borrow = 0;
        int_res[max_int - 1 - i] = da - db;
    }

    result->decimal = number_value_value_new_empty();
    ull_t start = 0;
    while (start < max_int - 1 && int_res[start] == 0) start++;
    for (ull_t i = start; i < max_int; i++)
        number_value_value_push_back(result->decimal, int_res[i]);
    free(int_res);
    number_value_normalize(result);
    return result;
}

ull_t *multiply_integer(const ull_t *left, ull_t a_len, const ull_t *right, ull_t b_len, ull_t *out_length)
{
    if (a_len == 0 || b_len == 0)
    {
        *out_length = 1;
        const auto result = (ull_t *) malloc(sizeof(ull_t));
        result[0] = 0;
        return result;
    }

    ull_t res_len = a_len + b_len;

    const auto result = (ull_t *) calloc(res_len, sizeof(ull_t));
    for (ull_t i = a_len; i > 0; i--)
    {
        ull_t carry = 0;
        for (ull_t j = b_len; j > 0; j--)
        {
            const ull_t pos = (i - 1) + (j - 1) + 1;
            const ull_t mul = left[i - 1] * right[j - 1] + result[pos] + carry;
            result[pos] = mul % 10;
            carry = mul / 10;
        }
        result[i - 1] += carry;
    }
    ull_t start = 0;
    while (start < res_len - 1 && result[start] == 0) start++;
    if (start > 0)
    {
        memmove(result, result + start, (res_len - start) * sizeof(ull_t));
        res_len -= start;
    }
    *out_length = res_len;
    return result;
}

static number_value_t *mul_abs(const number_value_t *left, const number_value_t *right)
{
    const ull_t frac_len_a = left->fraction->length;
    const ull_t frac_len_b = right->fraction->length;
    const ull_t total_frac_len = frac_len_a + frac_len_b;
    const ull_t int_len_a = left->decimal->length + frac_len_a;

    const auto int_a = (ull_t *)calloc(int_len_a, sizeof(ull_t));
    ull_t pos = 0;

    for (ull_t i = 0; i < left->decimal->length; i++) int_a[pos++] = left->decimal->data[i];
    for (ull_t i = 0; i < frac_len_a; i++) int_a[pos++] = left->fraction->data[i];

    const ull_t int_len_b = right->decimal->length + frac_len_b;
    const auto int_b = (ull_t *)calloc(int_len_b, sizeof(ull_t));

    pos = 0;

    for (ull_t i = 0; i < right->decimal->length; i++) int_b[pos++] = right->decimal->data[i];
    for (ull_t i = 0; i < frac_len_b; i++) int_b[pos++] = right->fraction->data[i];

    ull_t prod_len;
    ull_t *prod = multiply_integer(int_a, int_len_a, int_b, int_len_b, &prod_len);

    if (prod_len < total_frac_len)
    {
        const auto new_prod = (ull_t *) calloc(total_frac_len, sizeof(ull_t));
        const ull_t offset = total_frac_len - prod_len;
        memcpy(new_prod + offset, prod, prod_len * sizeof(ull_t));
        free(prod);
        prod = new_prod;
        prod_len = total_frac_len;
    }

    const ull_t int_part_len = prod_len - total_frac_len;

    const auto result = (number_value_t *) malloc(sizeof(number_value_t));

    result->negative = false;
    result->decimal = number_value_value_new_empty();

    if (int_part_len > 0)
    {
        for (ull_t i = 0; i < int_part_len; i++)
            number_value_value_push_back(result->decimal, prod[i]);
    } else
        number_value_value_push_back(result->decimal, 0);
    result->fraction = number_value_value_new_empty();
    for (ull_t i = int_part_len; i < prod_len; i++)
        number_value_value_push_back(result->fraction, prod[i]);
    free(int_a);
    free(int_b);
    free(prod);
    number_value_normalize(result);
    return result;
}

number_value_t *number_value_add(const number_value_t *left, const number_value_t *right)
{
    if (!left || !right) return nullptr;
    number_value_t *result = nullptr;
    if (left->negative == right->negative)
    {
        result = number_value_add_abs(left, right);
        result->negative = left->negative;
    }
    else
    {
        const int compare = number_value_compare_abs(left, right);
        if (compare > 0)
        {
            result = number_value_sub_abs(left, right);
            result->negative = left->negative;
        }
        else if (compare < 0)
        {
            result = number_value_sub_abs(right, left);
            result->negative = right->negative;
        }
        else
        {
            result = number_value_new(string_view_from("0"), string_view_from(""));
            result->negative = false;
        }
    }
    number_value_normalize(result);
    return result;
}

number_value_t *number_value_sub(const number_value_t *left, const number_value_t *right)
{
    if (!left || !right) return nullptr;

    number_value_t *neg_b = number_value_negate(right);
    number_value_t *result = number_value_add(left, neg_b);
    number_value_free(neg_b);

    return result;
}

number_value_t *number_value_mul(const number_value_t *left, const number_value_t *right)
{
    if (!left || !right) return nullptr;

    number_value_t *result = mul_abs(left, right);
    result->negative = left->negative ^ right->negative;
    number_value_normalize(result);

    return result;
}

number_value_t *number_value_negate(const number_value_t *number_value)
{
    if (!number_value) return nullptr;
    const auto result = (number_value_t *) malloc(sizeof(number_value_t));
    result->negative = !number_value->negative;
    result->decimal = number_value_value_clone(number_value->decimal);
    result->fraction = number_value_value_clone(number_value->fraction);
    return result;
}

static number_value_t* number_value_clone_full(const number_value_t* src)
{
    if (!src) return nullptr;
    const auto clone = (number_value_t*)malloc(sizeof(number_value_t));
    clone->negative = src->negative;
    clone->decimal = number_value_value_clone(src->decimal);
    clone->fraction = number_value_value_clone(src->fraction);
    return clone;
}

static void shift_decimal_right(const number_value_t* num, const ull_t K)
{
    if (K == 0) return;
    if (num->fraction->length >= K)
    {
        for (ull_t i = 0; i < K; i++)
            number_value_value_push_back(num->decimal, num->fraction->data[i]);

        memmove(num->fraction->data, num->fraction->data + K, (num->fraction->length - K) * sizeof(ull_t));
        num->fraction->length -= K;
        num->fraction->size = num->fraction->length;
    }
    else
    {
        ull_t to_move = num->fraction->length;
        for (ull_t i = 0; i < to_move; i++)
            number_value_value_push_back(num->decimal, num->fraction->data[i]);

        ull_t zeros_to_add = K - to_move;
        for (ull_t i = 0; i < zeros_to_add; i++)
            number_value_value_push_back(num->decimal, 0);

        num->fraction->length = 0;
        num->fraction->size = 0;
    }
    number_value_value_trim_lead(num->decimal);
}

static void shift_left_and_add_digit(number_value_value_t* val, ull_t digit)
{
    if (val->length == 1 && val->data[0] == 0)
        val->data[0] = digit;
    else
        number_value_value_push_back(val, digit);
}

static int compare_rem_div(const number_value_value_t* rem, const number_value_value_t* div)
{
    ull_t r_start = 0;
    while (r_start < rem->length - 1 && rem->data[r_start] == 0) r_start++;
    ull_t d_start = 0;
    while (d_start < div->length - 1 && div->data[d_start] == 0) d_start++;

    ull_t r_len = rem->length - r_start;
    ull_t d_len = div->length - d_start;

    if (r_len != d_len) return (r_len > d_len) ? 1 : -1;
    for (ull_t i = 0; i < r_len; i++)
    {
        if (rem->data[r_start + i] != div->data[d_start + i])
            return (rem->data[r_start + i] > div->data[d_start + i]) ? 1 : -1;
    }
    return 0;
}

static void sub_in_place(number_value_value_t* rem, const number_value_value_t* div)
{
    const ull_t r_len = rem->length;
    const ull_t d_len = div->length;
    ull_t borrow = 0;

    for (ull_t i = 0; i < d_len; i++)
    {
        const ull_t r_idx = r_len - 1 - i;
        const ull_t d_idx = d_len - 1 - i;
        ull_t r_val = rem->data[r_idx];
        const ull_t d_val = div->data[d_idx] + borrow;

        if (r_val < d_val) { r_val += 10; borrow = 1; }
        else borrow = 0;

        rem->data[r_idx] = r_val - d_val;
    }

    ull_t i = d_len;
    while (borrow && i < r_len)
    {
        const ull_t r_idx = r_len - 1 - i;
        if (rem->data[r_idx] == 0) { rem->data[r_idx] = 9; borrow = 1; }
        else { rem->data[r_idx]--; borrow = 0; }
        i++;
    }
    number_value_value_trim_lead(rem);
}

number_value_t* number_value_div(const number_value_t* left, const number_value_t* right)
{
    if (!left || !right) return nullptr;

    const bool left_is_zero = (right->decimal->length == 1 && right->decimal->data[0] == 0 && right->fraction->length == 0);
    if (left_is_zero) THROW("Division by zero is not allowed\n");

    const bool right_is_zero = (left->decimal->length == 1 && left->decimal->data[0] == 0 && left->fraction->length == 0);
    if (right_is_zero) return number_value_new(string_view_from("0"), string_view_from(""));

    const bool res_neg = left->negative ^ right->negative;

    number_value_t* A = number_value_clone_full(left);
    number_value_t* B = number_value_clone_full(right);

    const ull_t K = B->fraction->length;
    shift_decimal_right(A, K);
    shift_decimal_right(B, K);

    number_value_value_t* rem = number_value_value_new_empty();
    number_value_value_push_back(rem, 0);

    number_value_value_t* q_dec = number_value_value_new_empty();

    for (ull_t i = 0; i < A->decimal->length; i++)
    {
        shift_left_and_add_digit(rem, A->decimal->data[i]);
        ull_t count = 0;
        while (compare_rem_div(rem, B->decimal) >= 0)
        {
            sub_in_place(rem, B->decimal);
            count++;
        }
        number_value_value_push_back(q_dec, count);
    }
    number_value_value_trim_lead(q_dec);

    number_value_value_t* q_frac = number_value_value_new_empty();
    const ull_t max_frac = interpret_info_get()->number_accurate;
    ull_t frac_count = 0;

    for (ull_t i = 0; i < A->fraction->length && frac_count < max_frac; i++)
    {
        shift_left_and_add_digit(rem, A->fraction->data[i]);
        ull_t count = 0;
        while (compare_rem_div(rem, B->decimal) >= 0)
        {
            sub_in_place(rem, B->decimal);
            count++;
        }
        number_value_value_push_back(q_frac, count);
        frac_count++;
    }

    while (frac_count < max_frac)
    {
        const bool rem_is_zero = (rem->length == 1 && rem->data[0] == 0);
        if (rem_is_zero) break;

        shift_left_and_add_digit(rem, 0);
        ull_t count = 0;
        while (compare_rem_div(rem, B->decimal) >= 0)
        {
            sub_in_place(rem, B->decimal);
            count++;
        }
        number_value_value_push_back(q_frac, count);
        frac_count++;
    }
    number_value_value_trim_trail(q_frac);

    const auto result = (number_value_t*)malloc(sizeof(number_value_t));
    result->negative = res_neg;
    result->decimal = q_dec;
    result->fraction = q_frac;

    const bool is_zero = (q_dec->length == 1 && q_dec->data[0] == 0 && q_frac->length == 0);
    if (is_zero) result->negative = false;

    number_value_normalize(result);

    number_value_free(A);
    number_value_free(B);
    free(rem->data);
    free(rem);

    return result;
}

number_value_t *number_value_from_long_double(const long double value)
{
    if (isnan(value) || isinf(value)) THROW("Cannot convert NaN or Inf to number_value");

    const bool negative = (value < 0.0);
    const long double abs_val = negative ? -value : value;
    char* buffer = alloca(interpret_info_get()->number_accurate);
    snprintf(buffer, interpret_info_get()->number_accurate * sizeof(char), "%.*Lf", (unsigned int)interpret_info_get()->number_accurate, abs_val);

    const char *dot = strchr(buffer, '.');
    string_view_t decimal_sv, fraction_sv;
    if (dot)
    {
        decimal_sv.data = buffer;
        decimal_sv.length = dot - buffer;
        fraction_sv.data = dot + 1;
        fraction_sv.length = strlen(dot + 1);
    }
    else
    {
        decimal_sv.data = buffer;
        decimal_sv.length = strlen(buffer);
        fraction_sv.data = "";
        fraction_sv.length = 0;
    }


    if (decimal_sv.length == 0)
    {
        decimal_sv.data = "0";
        decimal_sv.length = 1;
    }

    number_value_t *num = number_value_new(decimal_sv, fraction_sv);
    num->negative = negative;
    number_value_normalize(num);
    return num;
}