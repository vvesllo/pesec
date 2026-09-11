#include "../include/number_value.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/utils/interpret_info.h"
#include "include/utils/throw.h"

static constexpr u32_t NV_P10[NV_LIMB_DIGITS + 1] = {
    1u, 10u, 100u, 1000u, 10000u, 100000u,
    1000000u, 10000000u, 100000000u, 1000000000u
};

static u64_t nv_top_digits(u32_t v)
{
    u64_t n = 1;
    while (v >= 10)
    {
        v /= 10;
        ++n;
    }
    return n;
}

static number_value_mantissa_t *nv_mantissa_new(u64_t size)
{
    auto m = (number_value_mantissa_t *) malloc(sizeof(number_value_mantissa_t));
    if (!m) return nullptr;
    m->capacity = size < 4 ? 4 : size;
    m->size = size;
    m->limb = (u32_t *) calloc(m->capacity, sizeof(u32_t));
    if (!m->limb)
    {
        free(m);
        return nullptr;
    }
    return m;
}

static void nv_mantissa_free(number_value_mantissa_t *m)
{
    if (!m) return;
    free(m->limb);
    free(m);
}

static bool nv_mantissa_reserve(number_value_mantissa_t *m, u64_t need)
{
    if (need <= m->capacity) return true;
    if (need > NV_MAX_LIMBS) return false;
    u64_t cap = m->capacity ? m->capacity : 4;
    while (cap < need) cap *= 2;
    auto p = (u32_t *) realloc(m->limb, cap * sizeof(u32_t));
    if (!p) return false;
    m->limb = p;
    m->capacity = cap;
    return true;
}

static bool nv_push(number_value_mantissa_t *m, u32_t limb)
{
    if (m->size >= m->capacity && !nv_mantissa_reserve(m, m->size + 1)) return false;
    m->limb[m->size++] = limb;
    return true;
}

static void nv_trim_top(number_value_mantissa_t *m)
{
    while (m->size > 1 && m->limb[m->size - 1] == 0) --m->size;
}

static bool nv_is_zero(const number_value_mantissa_t *m)
{
    for (u64_t i = 0; i < m->size; ++i)
        if (m->limb[i]) return false;
    return true;
}

static u64_t nv_dec_digits(const number_value_mantissa_t *m)
{
    u64_t i = m->size;
    while (i > 1 && m->limb[i - 1] == 0) --i;
    return (i - 1) * NV_LIMB_DIGITS + nv_top_digits(m->limb[i - 1]);
}

static number_value_mantissa_t *nv_mantissa_clone(const number_value_mantissa_t *src)
{
    auto m = nv_mantissa_new(src->size);
    if (!m) return nullptr;
    memcpy(m->limb, src->limb, src->size * sizeof(u32_t));
    return m;
}

/* цифра (0..9) в позиции pos от младшей */
static u32_t nv_digit_at(const number_value_mantissa_t *m, u64_t pos)
{
    if (pos >= nv_dec_digits(m)) return 0;
    return (m->limb[pos / NV_LIMB_DIGITS] / NV_P10[pos % NV_LIMB_DIGITS]) % 10u;
}

static void nv_increment(number_value_mantissa_t *m)
{
    for (u64_t i = 0; i < m->size; ++i)
    {
        if (++m->limb[i] < NV_LIMB_BASE) return;
        m->limb[i] = 0;
    }
    (void) nv_push(m, 1); /* резерв делает вызывающий; OOM здесь неотловим */
}

/* /= 10^t */
static void nv_shr_digits(number_value_mantissa_t *m, u64_t t)
{
    const u64_t q = t / NV_LIMB_DIGITS;
    const u64_t r = t % NV_LIMB_DIGITS;
    if (q >= m->size)
    {
        m->limb[0] = 0;
        m->size = 1;
        return;
    }
    if (q)
    {
        memmove(m->limb, m->limb + q, (m->size - q) * sizeof(u32_t));
        m->size -= q;
    }
    if (r)
    {
        u64_t rem = 0;
        for (u64_t i = m->size; i-- > 0;)
        {
            const u64_t cur = rem * NV_LIMB_BASE + m->limb[i];
            m->limb[i] = (u32_t) (cur / NV_P10[r]);
            rem = cur % NV_P10[r];
        }
    }
    nv_trim_top(m);
}

/* *= 10^t */
static bool nv_shl_digits(number_value_mantissa_t *m, u64_t t)
{
    const u64_t q = t / NV_LIMB_DIGITS;
    const u64_t r = t % NV_LIMB_DIGITS;
    if (!nv_mantissa_reserve(m, m->size + q + 1)) return false;
    if (q)
    {
        memmove(m->limb + q, m->limb, m->size * sizeof(u32_t));
        memset(m->limb, 0, q * sizeof(u32_t));
        m->size += q;
    }
    if (r)
    {
        u64_t carry = 0;
        for (u64_t i = 0; i < m->size; ++i)
        {
            const u64_t cur = (u64_t) m->limb[i] * NV_P10[r] + carry;
            m->limb[i] = (u32_t) (cur % NV_LIMB_BASE);
            carry = cur / NV_LIMB_BASE;
        }
        if (carry) m->limb[m->size++] = (u32_t) carry;
    }
    return true;
}

/* *= f, f < NV_LIMB_BASE */
static bool nv_scale_small(number_value_mantissa_t *m, u32_t f)
{
    if (!f)
    {
        m->limb[0] = 0;
        m->size = 1;
        return true;
    }
    if (!nv_mantissa_reserve(m, m->size + 1)) return false;
    u64_t carry = 0;
    for (u64_t i = 0; i < m->size; ++i)
    {
        const u64_t cur = (u64_t) m->limb[i] * f + carry;
        m->limb[i] = (u32_t) (cur % NV_LIMB_BASE);
        carry = cur / NV_LIMB_BASE;
    }
    if (carry) m->limb[m->size++] = (u32_t) carry;
    return true;
}

/* j-й лимб значения, выровненного сдвигом sh десятичных цифр влево */
static u32_t nv_limb_shifted(const number_value_mantissa_t *m, u64_t sh, u64_t j)
{
    const i64_t base = (i64_t) (NV_LIMB_DIGITS * j) - (i64_t) sh;
    if (base + (i64_t) NV_LIMB_DIGITS <= 0) return 0;
    if (base >= (i64_t) (NV_LIMB_DIGITS * m->size)) return 0;
    if (base < 0) /* окно начинается ниже нуля: младшие цифры limb[0] */
        return (u32_t) ((u64_t) (m->limb[0] % NV_P10[base + NV_LIMB_DIGITS]) * NV_P10[-base]);
    const u64_t li = (u64_t) base / NV_LIMB_DIGITS;
    const u64_t r = (u64_t) base % NV_LIMB_DIGITS;
    if (r == 0) return li < m->size ? m->limb[li] : 0;
    const u64_t hi = li < m->size ? m->limb[li] : 0;
    const u64_t lo = (li + 1 < m->size) ? m->limb[li + 1] : 0;
    return (u32_t) (hi / NV_P10[r] + (lo % NV_P10[r]) * NV_P10[NV_LIMB_DIGITS - r]);
}

static int nv_cmp_shifted(const number_value_mantissa_t *a, u64_t sha,
                          const number_value_mantissa_t *b, u64_t shb)
{
    const u64_t da = nv_dec_digits(a) + sha;
    const u64_t db = nv_dec_digits(b) + shb;
    if (da != db) return da < db ? -1 : 1;
    for (u64_t d = da; d-- > 0;)
    {
        const u32_t x = d >= sha ? nv_digit_at(a, d - sha) : 0;
        const u32_t y = d >= shb ? nv_digit_at(b, d - shb) : 0;
        if (x != y) return x < y ? -1 : 1;
    }
    return 0;
}

static int nv_cmp_limbs(const number_value_mantissa_t *a, const number_value_mantissa_t *b)
{
    u64_t sa = number_value_mantissa_significant_size(a);
    u64_t sb = number_value_mantissa_significant_size(b);
    if (sa != sb) return sa < sb ? -1 : 1;
    for (u64_t i = sa; i-- > 0;)
        if (a->limb[i] != b->limb[i]) return a->limb[i] < b->limb[i] ? -1 : 1;
    return 0;
}

static number_value_mantissa_t *nv_add_shifted(const number_value_mantissa_t *a, u64_t sha,
                                               const number_value_mantissa_t *b, u64_t shb)
{
    const u64_t na = a->size + (sha + NV_LIMB_DIGITS - 1) / NV_LIMB_DIGITS;
    const u64_t nb = b->size + (shb + NV_LIMB_DIGITS - 1) / NV_LIMB_DIGITS;
    const u64_t n = (na > nb ? na : nb) + 1;
    auto res = nv_mantissa_new(n);
    if (!res) return nullptr;
    u32_t carry = 0;
    for (u64_t j = 0; j < n; ++j)
    {
        const u32_t s = nv_limb_shifted(a, sha, j) + nv_limb_shifted(b, shb, j) + carry;
        carry = s >= NV_LIMB_BASE;
        res->limb[j] = carry ? s - NV_LIMB_BASE : s;
    }
    nv_trim_top(res);
    return res;
}

/* a >= b по модулю — обязанность вызывающего */
static number_value_mantissa_t *nv_sub_shifted(const number_value_mantissa_t *a, u64_t sha,
                                               const number_value_mantissa_t *b, u64_t shb)
{
    const u64_t na = a->size + (sha + NV_LIMB_DIGITS - 1) / NV_LIMB_DIGITS;
    const u64_t nb = b->size + (shb + NV_LIMB_DIGITS - 1) / NV_LIMB_DIGITS;
    const u64_t n = na > nb ? na : nb;
    auto res = nv_mantissa_new(n);
    if (!res) return nullptr;
    int64_t borrow = 0;
    for (u64_t j = 0; j < n; ++j)
    {
        int64_t t = (int64_t) nv_limb_shifted(a, sha, j)
                    - (int64_t) nv_limb_shifted(b, shb, j) - borrow;
        if (t < 0)
        {
            t += NV_LIMB_BASE;
            borrow = 1;
        } else borrow = 0;
        res->limb[j] = (u32_t) t;
    }
    nv_trim_top(res);
    return res;
}

/* ===================== нормализация ===================== */

static void nv_normalize(number_value_t *v, const u64_t acc)
{
    const auto m = v->mantissa;
    nv_trim_top(m);

    /* усечение дробной части до acc цифр, round-half-up по модулю */
    if (v->exponent > acc)
    {
        const u64_t excess = v->exponent - acc;
        const u64_t digs = nv_dec_digits(m);
        const u32_t rd = nv_digit_at(m, excess >= digs ? digs - 1 : excess - 1);
        if (excess >= digs)
        {
            m->limb[0] = 0;
            m->size = 1;
        } else nv_shr_digits(m, excess);
        v->exponent = acc;
        (void) nv_mantissa_reserve(m, m->size + 1);
        if (rd >= 5) nv_increment(m);
        nv_trim_top(m);
    }

    /* хвостовые нули дробной части -> уменьшение exponent (как в исходнике) */
    while (v->exponent > 0 && !nv_is_zero(m))
    {
        const u64_t digs = nv_dec_digits(m);
        u32_t low = m->limb[0];
        u64_t tz = 0;
        while (tz < NV_LIMB_DIGITS && low % 10 == 0)
        {
            low /= 10;
            ++tz;
        }
        u64_t t = tz < v->exponent ? tz : v->exponent;
        const u64_t cap = digs - 1; /* хотя бы одна цифра остаётся */
        if (t > cap) t = cap;
        if (t == 0) break;
        nv_shr_digits(m, t);
        v->exponent -= t;
    }

    if (nv_is_zero(m))
    {
        m->limb[0] = 0;
        m->size = 1;
        v->negative = false;
        v->exponent = 0;
    }
}

/* ===================== публичный слой мантиссы ===================== */

u32_t number_value_mantissa_get_limb(const number_value_mantissa_t *m, u64_t i)
{
    return i < m->size ? m->limb[i] : 0;
}

void number_value_mantissa_set_limb(const number_value_mantissa_t *m, const u64_t index, const u32_t limb)
{
    if (index < m->size) m->limb[index] = limb % NV_LIMB_BASE;
}

void number_value_mantissa_push_limb(number_value_mantissa_t *m, const u32_t limb)
{
    if (!nv_push(m, limb % NV_LIMB_BASE))
        THROW("Failed to realloc mantissa data");
}

u64_t number_value_mantissa_significant_size(const number_value_mantissa_t *m)
{
    u64_t i = m->size;
    while (i > 0 && m->limb[i - 1] == 0) --i;
    return i;
}

/* ===================== конструирование ===================== */

number_value_t *number_value_new(number_value_mantissa_t *mantissa, const u64_t exponent)
{
    auto v = (number_value_t *) malloc(sizeof(number_value_t));
    if (!v) return nullptr;
    v->negative = false;
    v->exponent = exponent;
    v->mantissa = mantissa;
    return v;
}

void number_value_free(number_value_t *v)
{
    if (!v) return;
    nv_mantissa_free(v->mantissa);
    free(v);
}

static number_value_t *nv_clone_v(const number_value_t *src)
{
    auto m = nv_mantissa_clone(src->mantissa);
    if (!m) return nullptr;
    auto v = number_value_new(m, src->exponent);
    if (!v)
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    v->negative = src->negative;
    return v;
}

static number_value_t *nv_from_small(u32_t limb, u64_t exponent)
{
    auto m = nv_mantissa_new(1);
    if (!m) return nullptr;
    m->limb[0] = limb;
    auto v = number_value_new(m, exponent);
    if (!v)
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    return v;
}

number_value_t *number_value_one(void)
{
    auto v = nv_from_small(1, 0);
    if (!v)
        THROW("Failed to alloc number value");
    return v;
}

number_value_t *number_value_zero(void)
{
    auto v = nv_from_small(0, 0);
    if (!v)
        THROW("Failed to alloc number value");
    return v;
}

/*
 * Разбор десятичных цифр из [s, s+len).
 * strict: нецифровой символ (кроме '.') -> false. Иначе — пропуск.
 * acc_limit: остановиться после (acc_limit+1)-й дробной цифры (семантика
 * исходного from_sv: лишняя цифра парсится и потом округляется normalize).
 * Ошибку аллокации тоже возвращает как false.
 */
static bool nv_parse_decimal(number_value_mantissa_t *out, const char *s, u64_t len,
                             bool strict, u64_t acc_limit, u64_t *exponent_out)
{
    u64_t count = 0, exponent = 0;
    bool floating = false;

    for (u64_t i = 0; i < len; ++i)
    {
        const char c = s[i];
        if (c == '.')
        {
            floating = true;
            continue;
        }
        if (c < '0' || c > '9')
        {
            if (strict) return false;
            continue;
        }
        ++count;
        if (floating) ++exponent;
        if (exponent > acc_limit) break;
    }

    /* упаковка по 9 цифр в лимб, выравнивание от младшего разряда */
    u64_t target = count % NV_LIMB_DIGITS;
    if (target == 0) target = NV_LIMB_DIGITS;

    u32_t acc = 0;
    u64_t cnt = 0;
    exponent = 0;
    floating = false;

    for (u64_t i = 0; i < len; ++i)
    {
        const char c = s[i];
        if (c == '.')
        {
            floating = true;
            continue;
        }
        if (c < '0' || c > '9')
        {
            if (strict) return false;
            continue;
        }
        acc = acc * 10u + (u32_t) (c - '0');
        ++cnt;
        if (floating) ++exponent;
        if (cnt == target)
        {
            if (!nv_push(out, acc)) return false;
            acc = 0;
            cnt = 0;
            target = NV_LIMB_DIGITS;
        }
        if (exponent > acc_limit) break;
    }
    if (cnt && !nv_push(out, acc)) return false;
    if (out->size == 0 && !nv_push(out, 0)) return false;

    *exponent_out = exponent;
    return true;
}

number_value_t *number_value_from_sv(const string_view_t sv)
{
    const u64_t acc = interpret_info_get()->number_accuracy;

    auto m = nv_mantissa_new(0);
    if (!m)
        THROW("Failed to alloc mantissa");

    u64_t exponent = 0;
    if (!nv_parse_decimal(m, sv.data, sv.length, true, acc, &exponent))
    {
        nv_mantissa_free(m);
        THROW("Invalid character in number literal");
    }

    auto v = number_value_new(m, exponent);
    if (!v)
    {
        nv_mantissa_free(m);
        THROW("Failed to alloc number value");
    }
    nv_normalize(v, acc);
    return v;
}

number_value_t *number_value_from_long_double(long double value)
{
    if (!isfinite(value))
        THROW("Cannot convert a non-finite value");

    const bool negative = value < 0.0L;
    if (negative) value = -value;

    const int fractional_digits = (int) interpret_info_get()->number_accuracy;
    const size_t buffer_size = (size_t) fractional_digits + 5200;

    char *buffer = (char *) malloc(buffer_size);
    if (!buffer)
        THROW("Failed to alloc conversion buffer");
    snprintf(buffer, buffer_size, "%.*Lf", fractional_digits, value);

    auto m = nv_mantissa_new(0);
    if (!m)
    {
        free(buffer);
        THROW("Failed to alloc mantissa");
    }

    u64_t exponent = 0;
    const bool ok = nv_parse_decimal(m, buffer, strlen(buffer), false, ULLONG_MAX, &exponent);
    free(buffer);
    if (!ok)
    {
        nv_mantissa_free(m);
        THROW("Failed to alloc mantissa");
    }

    auto v = number_value_new(m, exponent);
    if (!v)
    {
        nv_mantissa_free(m);
        THROW("Failed to alloc number value");
    }
    v->negative = negative;
    nv_normalize(v, interpret_info_get()->number_accuracy);
    return v;
}

long double number_value_to_long_double(const number_value_t *v)
{
    long double r = 0.0L;
    const auto m = v->mantissa;
    for (u64_t i = m->size; i-- > 0;)
        r = r * (long double) NV_LIMB_BASE + (long double) m->limb[i];
    if (v->exponent)
        r /= powl(10.0L, (long double) v->exponent);
    return v->negative ? -r : r;
}

char *number_value_to_string(const number_value_t *v) /* линейная печать */
{
    const auto m = v->mantissa;
    const u64_t sig = number_value_mantissa_significant_size(m);
    const u64_t digs = nv_dec_digits(m);
    const u64_t exp = v->exponent;

    char *digits = (char *) malloc(digs + 1);
    if (!digits) return nullptr;
    int off = sprintf(digits, "%u", (unsigned) m->limb[sig - 1]);
    for (u64_t i = sig - 1; i-- > 0;)
        off += sprintf(digits + off, "%09u", (unsigned) m->limb[i]);

    const u64_t sign = v->negative ? 1 : 0;
    char *out;
    u64_t o = 0;

    if (exp == 0)
    {
        out = (char *) malloc(sign + digs + 1);
        if (!out)
        {
            free(digits);
            return nullptr;
        }
        if (sign) out[o++] = '-';
        memcpy(out + o, digits, digs);
        o += digs;
    } else if (exp < digs)
    {
        out = (char *) malloc(sign + digs + 1 + 1);
        if (!out)
        {
            free(digits);
            return nullptr;
        }
        if (sign) out[o++] = '-';
        memcpy(out + o, digits, digs - exp);
        o += digs - exp;
        out[o++] = '.';
        memcpy(out + o, digits + digs - exp, exp);
        o += exp;
    } else
    {
        /* 0.000<digits> */
        out = (char *) malloc(sign + 2 + exp + 1);
        if (!out)
        {
            free(digits);
            return nullptr;
        }
        if (sign) out[o++] = '-';
        out[o++] = '0';
        out[o++] = '.';
        memset(out + o, '0', exp - digs);
        o += exp - digs;
        memcpy(out + o, digits, digs);
        o += digs;
    }
    out[o] = '\0';
    free(digits);
    return out;
}

/* ===================== сравнение ===================== */

int number_value_compare_abs(const number_value_t *l, const number_value_t *r)
{
    const u64_t e = l->exponent > r->exponent ? l->exponent : r->exponent;
    return nv_cmp_shifted(l->mantissa, e - l->exponent, r->mantissa, e - r->exponent);
}

short number_value_compare(const number_value_t *l, const number_value_t *r)
{
    const bool lz = nv_is_zero(l->mantissa);
    const bool rz = nv_is_zero(r->mantissa);
    if (lz && rz) return 0;
    if (lz) return r->negative ? 1 : -1;
    if (rz) return l->negative ? -1 : 1;
    if (l->negative != r->negative) return l->negative ? -1 : 1;
    const int c = number_value_compare_abs(l, r);
    return (short) (l->negative ? -c : c);
}

unsigned char number_value_aligned_digit_from_lsb(const number_value_t *v,
                                                  u64_t aligned_exponent, u64_t index)
{
    if (aligned_exponent < v->exponent) return 0; /* контракт: aligned >= v->exponent */
    const u64_t shift = aligned_exponent - v->exponent;
    if (index < shift) return 0;
    return (unsigned char) nv_digit_at(v->mantissa, index - shift);
}

/* ===================== сложение / вычитание ===================== */

static number_value_t *nv_add_v(const number_value_t *l, const number_value_t *r, u64_t acc)
{
    const u64_t e = l->exponent > r->exponent ? l->exponent : r->exponent;
    const u64_t ls = e - l->exponent;
    const u64_t rs = e - r->exponent;

    number_value_mantissa_t *m;
    bool negative;

    if (l->negative == r->negative)
    {
        negative = l->negative;
        m = nv_add_shifted(l->mantissa, ls, r->mantissa, rs);
    } else
    {
        const int c = nv_cmp_shifted(l->mantissa, ls, r->mantissa, rs);
        if (c == 0)
        {
            negative = false;
            m = nv_mantissa_new(1);
        } else if (c > 0)
        {
            negative = l->negative;
            m = nv_sub_shifted(l->mantissa, ls, r->mantissa, rs);
        } else
        {
            negative = r->negative;
            m = nv_sub_shifted(r->mantissa, rs, l->mantissa, ls);
        }
    }
    if (!m) return nullptr;

    auto v = number_value_new(m, e);
    if (!v)
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    v->negative = negative;
    nv_normalize(v, acc);
    return v;
}

number_value_t *number_value_add(const number_value_t *l, const number_value_t *r)
{
    auto v = nv_add_v(l, r, interpret_info_get()->number_accuracy);
    if (!v)
        THROW("Failed to alloc number value");
    return v;
}

static number_value_t *nv_negate_v(const number_value_t *v)
{
    auto n = nv_clone_v(v);
    if (!n) return nullptr;
    n->negative = !v->negative;
    return n;
}

number_value_t *number_value_negate(const number_value_t *v)
{
    auto n = nv_negate_v(v);
    if (!n)
        THROW("Failed to alloc number value");
    nv_normalize(n, interpret_info_get()->number_accuracy);
    return n;
}

static number_value_t *nv_sub_v(const number_value_t *l, const number_value_t *r, u64_t acc)
{
    auto nr = nv_negate_v(r);
    if (!nr) return nullptr;
    auto res = nv_add_v(l, nr, acc);
    number_value_free(nr);
    return res;
}

number_value_t *number_value_sub(const number_value_t *l, const number_value_t *r)
{
    auto v = nv_sub_v(l, r, interpret_info_get()->number_accuracy);
    if (!v)
        THROW("Failed to alloc number value");
    return v;
}

/* ===================== умножение: schoolbook + Карацуба ===================== */

#define NV_KARATSUBA_THRESHOLD 32

static void nv_mul_schoolbook(const u32_t *a, u64_t na, const u32_t *b, u64_t nb, u32_t *out)
{
    for (u64_t i = 0; i < na; ++i)
    {
        const u64_t ai = a[i];
        if (!ai) continue;
        u64_t carry = 0;
        for (u64_t j = 0; j < nb; ++j)
        {
            const u64_t cur = out[i + j] + ai * b[j] + carry; /* < 10^18 < 2^63 */
            out[i + j] = (u32_t) (cur % NV_LIMB_BASE);
            carry = cur / NV_LIMB_BASE;
        }
        out[i + nb] = (u32_t) carry;
    }
}

static void nv_limbs_add_at(u32_t *dst, const u32_t *src, u64_t n)
{
    u32_t carry = 0;
    for (u64_t j = 0; j < n; ++j)
    {
        const u32_t s = dst[j] + src[j] + carry;
        carry = s >= NV_LIMB_BASE;
        dst[j] = carry ? s - NV_LIMB_BASE : s;
    }
    for (u64_t k = n; carry; ++k)
    {
        const u32_t s = dst[k] + 1;
        carry = s >= NV_LIMB_BASE;
        dst[k] = carry ? s - NV_LIMB_BASE : s;
    }
}

static void nv_limbs_sub_at(u32_t *dst, const u32_t *src, u64_t n)
{
    int64_t borrow = 0;
    for (u64_t j = 0; j < n; ++j)
    {
        int64_t t = (int64_t) dst[j] - (int64_t) src[j] - borrow;
        if (t < 0)
        {
            t += NV_LIMB_BASE;
            borrow = 1;
        } else borrow = 0;
        dst[j] = (u32_t) t;
    }
    for (u64_t k = n; borrow; ++k)
    {
        int64_t t = (int64_t) dst[k] - borrow;
        if (t < 0)
        {
            t += NV_LIMB_BASE;
            borrow = 1;
        } else borrow = 0;
        dst[k] = (u32_t) t;
    }
}

/* out[0..na+nb) = a*b; out обязан быть нулевым; tmp >= 8*(na+nb)+64 лимбов */
static void nv_mul_limbs(const u32_t *a, u64_t na, const u32_t *b, u64_t nb,
                         u32_t *out, u32_t *tmp)
{
    if (na < nb)
    {
        const u32_t *p = a;
        a = b;
        b = p;
        u64_t s = na;
        na = nb;
        nb = s;
    }
    if (nb == 0) return;
    if (na <= NV_KARATSUBA_THRESHOLD)
    {
        nv_mul_schoolbook(a, na, b, nb, out);
        return;
    }

    const u64_t m = (na + 1) / 2;

    if (nb <= m)
    {
        /* a = a1*B^m + a0; out = a0*b + (a1*b << m) */
        u32_t *t = tmp;
        memset(t, 0, (m + nb) * sizeof(u32_t));
        nv_mul_limbs(a, m, b, nb, t, tmp + (m + nb));
        nv_limbs_add_at(out, t, m + nb);

        memset(t, 0, (na - m + nb) * sizeof(u32_t));
        nv_mul_limbs(a + m, na - m, b, nb, t, tmp + (na - m + nb));
        nv_limbs_add_at(out + m, t, na - m + nb);
        return;
    }

    /* сбалансированный случай: na, nb > m */
    u32_t *sa = tmp;
    u32_t *sb = tmp + (m + 1);
    u32_t *mid = tmp + 2 * (m + 1);
    u32_t *tmp2 = mid + (2 * m + 2);

    nv_mul_limbs(a, m, b, m, out, tmp); /* z0 -> out[0..2m)   */
    nv_mul_limbs(a + m, na - m, b + m, nb - m, out + 2 * m, tmp); /* z2 -> out[2m..n)   */

    u32_t ca = 0, cb = 0;
    for (u64_t i = 0; i < m; ++i)
    {
        u32_t s = a[i] + (m + i < na ? a[m + i] : 0) + ca;
        ca = s >= NV_LIMB_BASE;
        sa[i] = ca ? s - NV_LIMB_BASE : s;
        s = b[i] + (m + i < nb ? b[m + i] : 0) + cb;
        cb = s >= NV_LIMB_BASE;
        sb[i] = cb ? s - NV_LIMB_BASE : s;
    }
    sa[m] = ca;
    sb[m] = cb;

    memset(mid, 0, (2 * m + 2) * sizeof(u32_t));
    nv_mul_limbs(sa, m + 1, sb, m + 1, mid, tmp2); /* mid = (a0+a1)(b0+b1) */

    nv_limbs_sub_at(mid, out, 2 * m); /* mid -= z0 */
    nv_limbs_sub_at(mid, out + 2 * m, (na - m) + (nb - m)); /* mid -= z2 */
    nv_limbs_add_at(out + m, mid, 2 * m + 2); /* out += mid << m */
}

number_value_t *number_value_mul(const number_value_t *l, const number_value_t *r)
{
    const u64_t acc = interpret_info_get()->number_accuracy;
    const auto a = l->mantissa;
    const auto b = r->mantissa;

    if (nv_is_zero(a) || nv_is_zero(b)) return number_value_zero();

    const u64_t n = a->size + b->size;
    u32_t *out = (u32_t *) calloc(n, sizeof(u32_t));
    u32_t *tmp = (u32_t *) malloc((8 * n + 64) * sizeof(u32_t));
    if (!out || !tmp)
    {
        free(out);
        free(tmp);
        THROW("Failed to alloc multiplication buffer");
    }

    nv_mul_limbs(a->limb, a->size, b->limb, b->size, out, tmp);
    free(tmp);

    auto m = (number_value_mantissa_t *) malloc(sizeof(number_value_mantissa_t));
    if (!m)
    {
        free(out);
        THROW("Failed to alloc mantissa");
    }
    m->limb = out;
    m->size = n;
    m->capacity = n;
    nv_trim_top(m);

    auto v = number_value_new(m, l->exponent + r->exponent);
    if (!v)
    {
        nv_mantissa_free(m);
        THROW("Failed to alloc number value");
    }
    v->negative = l->negative != r->negative;
    nv_normalize(v, acc);
    return v;
}

/* ===================== деление (Кнут D) ===================== */

/*
 * Q = trunc(|L| * 10^K / |R|),  K = acc + re - le.
 * Возвращает |Q| с exponent = acc. rem_out (если не nullptr) — остаток.
 * Ошибки аллокации -> nullptr (никаких THROW внутри: утечек нет по построению).
 */
static number_value_t *nv_divmod_abs(const number_value_t *l, const number_value_t *r,
                                     bool round_half_up, bool want_rem,
                                     number_value_mantissa_t **rem_out)
{
    const u64_t acc = interpret_info_get()->number_accuracy;
    const i64_t k = (i64_t) acc + (i64_t) r->exponent - (i64_t) l->exponent;

    number_value_t *result = nullptr;
    number_value_mantissa_t *q = nullptr;
    number_value_mantissa_t *rem = nullptr;
    number_value_mantissa_t *num = nv_mantissa_clone(l->mantissa);
    number_value_mantissa_t *den = nv_mantissa_clone(r->mantissa);
    if (!num || !den) goto fail;

    if (k >= 0) { if (!nv_shl_digits(num, (u64_t) k)) goto fail; } else
    {
        if (!nv_shl_digits(den, (u64_t) -k)) goto fail;
    }
    nv_trim_top(num);
    nv_trim_top(den);

    const u64_t n = den->size;
    const u64_t un = num->size;

    q = nv_mantissa_new(un > n ? un - n + 1 : 1);
    rem = nv_mantissa_new(n);
    if (!q || !rem) goto fail;

    if (n > un)
    {
        q->limb[0] = 0;
        memcpy(rem->limb, num->limb, num->size * sizeof(u32_t));
        rem->size = num->size;
    } else
    {
        /* нормализация: v[n-1] >= BASE/2 */
        u32_t *u = (u32_t *) calloc(un + 1, sizeof(u32_t));
        u32_t *v = (u32_t *) malloc(n * sizeof(u32_t));
        if (!u || !v)
        {
            free(u);
            free(v);
            goto fail;
        }

        const u32_t dnorm = (u32_t) ((u64_t) NV_LIMB_BASE / ((u64_t) den->limb[n - 1] + 1));
        u64_t carry = 0;
        for (u64_t i = 0; i < un; ++i)
        {
            const u64_t cur = (u64_t) num->limb[i] * dnorm + carry;
            u[i] = (u32_t) (cur % NV_LIMB_BASE);
            carry = cur / NV_LIMB_BASE;
        }
        u[un] = (u32_t) carry;
        carry = 0;
        for (u64_t i = 0; i < n; ++i)
        {
            const u64_t cur = (u64_t) den->limb[i] * dnorm + carry;
            v[i] = (u32_t) (cur % NV_LIMB_BASE);
            carry = cur / NV_LIMB_BASE;
        }
        /* carry сюда доходит нулевым благодаря выбору dnorm */

        if (n == 1)
        {
            u64_t rmd = 0;
            for (u64_t i = un + 1; i-- > 0;)
            {
                const u64_t cur = rmd * NV_LIMB_BASE + u[i];
                q->limb[i] = (u32_t) (cur / v[0]);
                rmd = cur % v[0];
            }
            rem->limb[0] = (u32_t) (rmd / dnorm); /* делится нацело */
            rem->size = 1;
        } else
        {
            for (u64_t j = un - n + 1; j-- > 0;)
            {
                const u64_t top = (u64_t) u[j + n] * NV_LIMB_BASE + u[j + n - 1];
                u64_t qhat = top / v[n - 1];
                u64_t rhat = top % v[n - 1];
                while (qhat >= NV_LIMB_BASE ||
                       qhat * v[n - 2] > rhat * NV_LIMB_BASE + u[j + n - 2])
                {
                    --qhat;
                    rhat += v[n - 1];
                    if (rhat >= NV_LIMB_BASE) break;
                }

                int64_t borrow = 0;
                for (u64_t i = 0; i < n; ++i)
                {
                    const u64_t p = qhat * v[i];
                    const u32_t plo = (u32_t) (p % NV_LIMB_BASE);
                    const u64_t phi = p / NV_LIMB_BASE;
                    const int64_t t = (int64_t) u[j + i] - (int64_t) plo - borrow;
                    if (t < 0)
                    {
                        u[j + i] = (u32_t) (t + NV_LIMB_BASE);
                        borrow = (int64_t) phi + 1;
                    } else
                    {
                        u[j + i] = (u32_t) t;
                        borrow = (int64_t) phi;
                    }
                }
                const int64_t t = (int64_t) u[j + n] - borrow;
                if (t < 0)
                {
                    /* qhat оказался на 1 больше: add-back */
                    --qhat;
                    u64_t c = 0;
                    for (u64_t i = 0; i < n; ++i)
                    {
                        const u64_t cur = (u64_t) u[j + i] + v[i] + c;
                        u[j + i] = (u32_t) (cur % NV_LIMB_BASE);
                        c = cur / NV_LIMB_BASE;
                    }
                    u[j + n] = (u32_t) (t + (int64_t) NV_LIMB_BASE + (int64_t) c);
                } else
                {
                    u[j + n] = (u32_t) t;
                }
                q->limb[j] = (u32_t) qhat;
            }
            /* денормализация остатка */
            u64_t rmd = 0;
            for (u64_t i = n; i-- > 0;)
            {
                const u64_t cur = rmd * NV_LIMB_BASE + u[i];
                rem->limb[i] = (u32_t) (cur / dnorm);
                rmd = cur % dnorm;
            }
            rem->size = n;
        }
        free(u);
        free(v);
    }
    nv_trim_top(q);
    nv_trim_top(rem);
    nv_mantissa_free(num);
    nv_mantissa_free(den);

    if (round_half_up && !nv_is_zero(rem))
    {
        /* 2*rem >= den ? */
        auto rem2 = nv_mantissa_clone(rem);
        if (!rem2)
        {
            nv_mantissa_free(q);
            nv_mantissa_free(rem);
            return nullptr;
        }
        const bool ok = nv_scale_small(rem2, 2);
        const bool up = ok && nv_cmp_limbs(rem2, den) >= 0;
        nv_mantissa_free(rem2);
        if (!ok)
        {
            nv_mantissa_free(q);
            nv_mantissa_free(rem);
            return nullptr;
        }
        if (up)
        {
            (void) nv_mantissa_reserve(q, q->size + 1);
            nv_increment(q);
        }
    }

    result = number_value_new(q, acc);
    if (!result)
    {
        nv_mantissa_free(q);
        nv_mantissa_free(rem);
        return nullptr;
    }

    if (want_rem) *rem_out = rem;
    else nv_mantissa_free(rem);
    return result;

fail:
    nv_mantissa_free(num);
    nv_mantissa_free(den);
    nv_mantissa_free(q);
    nv_mantissa_free(rem);
    return nullptr;
}

static number_value_t *nv_div_v(const number_value_t *l, const number_value_t *r, u64_t acc)
{
    if (nv_is_zero(r->mantissa))
        THROW("Division by zero");

    auto v = nv_divmod_abs(l, r, true, false, nullptr);
    if (!v) return nullptr;
    v->negative = l->negative != r->negative;
    nv_normalize(v, acc);
    return v;
}

number_value_t *number_value_div(const number_value_t *l, const number_value_t *r)
{
    auto v = nv_div_v(l, r, interpret_info_get()->number_accuracy);
    if (!v)
        THROW("Failed to alloc number value");
    return v;
}

number_value_t *number_value_floor_div(const number_value_t *l, const number_value_t *r)
{
    const u64_t acc = interpret_info_get()->number_accuracy;
    if (nv_is_zero(r->mantissa))
        THROW("Division by zero");

    number_value_mantissa_t *rem = nullptr;
    auto q = nv_divmod_abs(l, r, false, true, &rem);
    if (!q)
        THROW("Failed to alloc number value");

    /* дробная часть есть, если ненулевой остаток ИЛИ Q mod 10^acc != 0
       (покрывает кейс "quotient обрезан до нуля, но остаток жив") */
    bool has_fraction = !nv_is_zero(rem);
    nv_mantissa_free(rem);

    const u64_t q9 = acc / NV_LIMB_DIGITS;
    const u64_t rr = acc % NV_LIMB_DIGITS;
    const auto qm = q->mantissa;
    for (u64_t i = 0; !has_fraction && i < q9 && i < qm->size; ++i)
        if (qm->limb[i]) has_fraction = true;
    if (!has_fraction && rr && q9 < qm->size && (qm->limb[q9] % NV_P10[rr]))
        has_fraction = true;

    nv_shr_digits(qm, acc); /* целая часть: Q / 10^acc */
    q->exponent = 0;
    q->negative = l->negative != r->negative;

    if (q->negative && has_fraction)
    {
        (void) nv_mantissa_reserve(qm, qm->size + 1);
        nv_increment(qm);
    }
    nv_normalize(q, acc);
    return q;
}

/* ===================== pow / sqrt / exp / ln ===================== */

static bool nv_is_integer(const number_value_t *v)
{
    if (nv_is_zero(v->mantissa)) return true;
    const u64_t digs = nv_dec_digits(v->mantissa);
    if (digs <= v->exponent) return false;
    for (u64_t p = 0; p < v->exponent; ++p)
        if (nv_digit_at(v->mantissa, p) != 0) return false;
    return true;
}

/* точное деление на малое d (d < NV_LIMB_BASE) с одной защитной цифрой */
static number_value_t *nv_div_small(const number_value_t *x, u32_t d, u64_t acc)
{
    auto m = nv_mantissa_clone(x->mantissa);
    if (!m) return nullptr;
    if (!nv_shl_digits(m, 1))
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    u64_t r = 0;
    for (u64_t i = m->size; i-- > 0;)
    {
        const u64_t cur = r * NV_LIMB_BASE + m->limb[i];
        m->limb[i] = (u32_t) (cur / d);
        r = cur % d;
    }
    nv_trim_top(m);
    auto v = number_value_new(m, x->exponent + 1);
    if (!v)
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    v->negative = x->negative;
    nv_normalize(v, acc);
    return v;
}

static bool nv_negligible(const number_value_t *term, const number_value_t *sum, u64_t acc)
{
    if (number_value_mantissa_significant_size(term->mantissa) == 0) return true;
    if (number_value_mantissa_significant_size(sum->mantissa) == 0) return false;
    const i64_t tm = (i64_t) nv_dec_digits(term->mantissa) - (i64_t) term->exponent;
    const i64_t sm = (i64_t) nv_dec_digits(sum->mantissa) - (i64_t) sum->exponent;
    return tm + (i64_t) acc + 3 < sm;
}

/* точное деление пополам: *= 5, exponent += 1 */
static number_value_t *nv_halve(const number_value_t *v, u64_t acc)
{
    auto m = nv_mantissa_clone(v->mantissa);
    if (!m) return nullptr;
    if (!nv_scale_small(m, 5))
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    auto r = number_value_new(m, v->exponent + 1);
    if (!r)
    {
        nv_mantissa_free(m);
        return nullptr;
    }
    r->negative = v->negative;
    nv_normalize(r, acc);
    return r;
}

static number_value_t *nv_sqrt(const number_value_t *value, u64_t acc)
{
    if (nv_is_zero(value->mantissa)) return number_value_zero();
    if (value->negative)
        THROW("sqrt of a negative number");

    const u64_t digs = nv_dec_digits(value->mantissa);
    const u64_t take = digs < 18 ? digs : 18;

    long double lead = 0.0L;
    for (u64_t p = digs; p-- > digs - take;)
        lead = lead * 10.0L + (long double) nv_digit_at(value->mantissa, p);

    i64_t shift = (i64_t) digs - (i64_t) value->exponent - (i64_t) take;
    while (shift % 2 != 0)
    {
        lead *= 10.0L;
        --shift;
    }

    auto x = number_value_from_long_double(sqrtl(lead));
    if (!x) return nullptr;

    const i64_t half = shift / 2;
    if (half > 0)
    {
        if (!nv_shl_digits(x->mantissa, (u64_t) half))
        {
            number_value_free(x);
            return nullptr;
        }
    } else if (half < 0)
    {
        x->exponent += (u64_t) (-half);
    }
    nv_normalize(x, acc);

    number_value_t *prev = nullptr;
    for (u64_t iteration = 0; iteration < 400; ++iteration)
    {
        auto quotient = nv_div_v(value, x, acc);
        if (!quotient)
        {
            number_value_free(x);
            number_value_free(prev);
            return nullptr;
        }
        auto sum = nv_add_v(x, quotient, acc);
        number_value_free(quotient);
        if (!sum)
        {
            number_value_free(x);
            number_value_free(prev);
            return nullptr;
        }
        auto next = nv_halve(sum, acc);
        number_value_free(sum);
        if (!next)
        {
            number_value_free(x);
            number_value_free(prev);
            return nullptr;
        }

        const int c0 = number_value_compare(next, x);
        const int c1 = prev ? number_value_compare(next, prev) : 1;

        number_value_free(prev);
        prev = x;
        x = next;

        if (c0 == 0 || c1 == 0) break;
    }
    number_value_free(prev);
    return x;
}

static number_value_t *nv_exp(const number_value_t *value, u64_t acc)
{
    if (nv_is_zero(value->mantissa)) return number_value_one();

    if (value->negative)
    {
        auto pos = nv_negate_v(value);
        if (!pos) return nullptr;
        auto r = nv_exp(pos, acc);
        number_value_free(pos);
        if (!r) return nullptr;
        auto one = number_value_one();
        if (!one)
        {
            number_value_free(r);
            return nullptr;
        }
        auto reciprocal = nv_div_v(one, r, acc);
        number_value_free(one);
        number_value_free(r);
        return reciprocal;
    }

    auto x = nv_clone_v(value);
    if (!x) return nullptr;
    u64_t squarings = 0;

    auto half = nv_from_small(5, 1); /* 0.5 */
    if (!half)
    {
        number_value_free(x);
        return nullptr;
    }
    while (number_value_compare(x, half) > 0)
    {
        auto reduced = nv_halve(x, acc);
        number_value_free(x);
        x = reduced;
        if (!x)
        {
            number_value_free(half);
            return nullptr;
        }
        ++squarings;
    }
    number_value_free(half);

    number_value_t *term = number_value_one();
    number_value_t *sum = number_value_one();
    if (!term || !sum)
    {
        number_value_free(term);
        number_value_free(sum);
        number_value_free(x);
        return nullptr;
    }

    for (u32_t coef = 1; coef < 100000; ++coef)
    {
        auto scaled = number_value_mul(term, x);
        number_value_free(term);
        if (!scaled)
        {
            number_value_free(sum);
            number_value_free(x);
            return nullptr;
        }
        term = nv_div_small(scaled, coef, acc);
        number_value_free(scaled);
        if (!term)
        {
            number_value_free(sum);
            number_value_free(x);
            return nullptr;
        }

        auto new_sum = nv_add_v(sum, term, acc);
        number_value_free(sum);
        sum = new_sum;
        if (!sum)
        {
            number_value_free(term);
            number_value_free(x);
            return nullptr;
        }

        if (nv_negligible(term, sum, acc)) break;
    }
    number_value_free(term);
    number_value_free(x);

    for (u64_t i = 0; i < squarings; ++i)
    {
        auto squared = number_value_mul(sum, sum);
        number_value_free(sum);
        sum = squared;
        if (!sum) return nullptr;
    }
    return sum;
}

static number_value_t *nv_ln(const number_value_t *value, u64_t acc)
{
    if (nv_is_zero(value->mantissa))
        THROW("Logarithm of zero");
    if (value->negative)
        THROW("Logarithm of a negative number");

    auto x = nv_clone_v(value);
    if (!x) return nullptr;
    u64_t squarings = 0;

    auto lower = nv_from_small(9, 1); /* 0.9 */
    auto upper = nv_from_small(11, 2); /* 1.1 */
    if (!lower || !upper)
    {
        number_value_free(x);
        number_value_free(lower);
        number_value_free(upper);
        return nullptr;
    }

    while (squarings < 1000 &&
           (number_value_compare(x, lower) < 0 || number_value_compare(x, upper) > 0))
    {
        auto root = nv_sqrt(x, acc);
        number_value_free(x);
        x = root;
        if (!x)
        {
            number_value_free(lower);
            number_value_free(upper);
            return nullptr;
        }
        ++squarings;
    }
    number_value_free(lower);
    number_value_free(upper);

    auto one = number_value_one();
    if (!one)
    {
        number_value_free(x);
        return nullptr;
    }
    auto numerator = nv_sub_v(x, one, acc);
    auto denominator = nv_add_v(x, one, acc);
    number_value_free(x);
    number_value_free(one);
    if (!numerator || !denominator)
    {
        number_value_free(numerator);
        number_value_free(denominator);
        return nullptr;
    }

    auto y = nv_div_v(numerator, denominator, acc);
    number_value_free(numerator);
    number_value_free(denominator);
    if (!y) return nullptr;

    auto y2 = number_value_mul(y, y);
    auto power = nv_clone_v(y);
    auto sum = nv_clone_v(y);
    number_value_free(y);
    if (!y2 || !power || !sum)
    {
        number_value_free(y2);
        number_value_free(power);
        number_value_free(sum);
        return nullptr;
    }

    for (u32_t coef = 3; coef < 100000; coef += 2)
    {
        auto next_power = number_value_mul(power, y2);
        number_value_free(power);
        power = next_power;
        if (!power)
        {
            number_value_free(sum);
            number_value_free(y2);
            return nullptr;
        }

        auto term = nv_div_small(power, coef, acc);
        if (!term)
        {
            number_value_free(power);
            number_value_free(sum);
            number_value_free(y2);
            return nullptr;
        }

        auto new_sum = nv_add_v(sum, term, acc);
        number_value_free(sum);
        sum = new_sum;

        const bool done = nv_negligible(term, sum, acc);
        number_value_free(term);
        if (!sum)
        {
            number_value_free(power);
            number_value_free(y2);
            return nullptr;
        }
        if (done) break;
    }

    number_value_free(power);
    number_value_free(y2);

    auto doubled = nv_add_v(sum, sum, acc);
    number_value_free(sum);
    sum = doubled;
    if (!sum) return nullptr;

    for (u64_t i = 0; i < squarings; ++i)
    {
        auto d2 = nv_add_v(sum, sum, acc);
        number_value_free(sum);
        sum = d2;
        if (!sum) return nullptr;
    }
    return sum;
}

number_value_t *number_value_pow(const number_value_t *l, const number_value_t *r)
{
    const u64_t acc = interpret_info_get()->number_accuracy;

    if (r->negative && nv_is_zero(l->mantissa))
        THROW("Zero raised to a negative power");

    if (nv_is_integer(r))
    {
        /* показатель разбираем ДО аллокаций — ничего не утекает при THROW */
        const auto rm = r->mantissa;
        const u64_t digs = nv_dec_digits(rm);
        const u64_t int_digits = digs > r->exponent ? digs - r->exponent : 0;

        u64_t exponent = 0;
        for (u64_t i = digs; i-- > digs - int_digits;)
        {
            const u32_t digit = nv_digit_at(rm, i);
            if (exponent > (ULLONG_MAX - digit) / 10)
                THROW("Exponent is too large");
            exponent = exponent * 10 + digit;
        }

        auto result = number_value_one();
        auto base = nv_clone_v(l);
        if (!result || !base)
        {
            number_value_free(result);
            number_value_free(base);
            THROW("Failed to alloc number value");
        }

        while (exponent > 0)
        {
            if (exponent & 1)
            {
                auto temp = number_value_mul(result, base);
                number_value_free(result);
                result = temp;
                if (!result)
                {
                    number_value_free(base);
                    THROW("Failed to alloc number value");
                }
            }
            exponent >>= 1;
            if (exponent > 0)
            {
                auto temp = number_value_mul(base, base);
                number_value_free(base);
                base = temp;
                if (!base)
                {
                    number_value_free(result);
                    THROW("Failed to alloc number value");
                }
            }
        }
        number_value_free(base);

        if (r->negative)
        {
            auto one = number_value_one();
            if (!one)
            {
                number_value_free(result);
                THROW("Failed to alloc number value");
            }
            auto reciprocal = nv_div_v(one, result, acc);
            number_value_free(one);
            number_value_free(result);
            result = reciprocal;
            if (!result)
                THROW("Failed to alloc number value");
        }
        return result;
    }

    if (nv_is_zero(l->mantissa)) return number_value_zero();
    if (l->negative)
        THROW("Negative number raised to a non-integer power");

    /* глобальный accuracy больше НЕ мутируется: guard прокидывается параметром */
    const u64_t guard = acc + 8;
    auto log_value = nv_ln(l, guard);
    if (!log_value)
        THROW("Failed to alloc number value");
    auto scaled = number_value_mul(r, log_value);
    number_value_free(log_value);
    if (!scaled)
        THROW("Failed to alloc number value");
    auto result = nv_exp(scaled, guard);
    number_value_free(scaled);
    if (!result)
        THROW("Failed to alloc number value");
    nv_normalize(result, acc);
    return result;
}
