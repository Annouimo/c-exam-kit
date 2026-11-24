/** @category 大整数、枚举及其他类型 */
/* Fixed-capacity signed decimal BigInt, no dynamic ownership.
 * EXAM_BIGINT_CAP is the maximum NUMBER OF DECIMAL DIGITS. */
#ifndef EXAM_BIGINT_CAP
#define EXAM_BIGINT_CAP 2048
#endif
/**
 * @public
 * @requires EXAM_BIGINT_CAP
 * @category 大整数、枚举及其他类型
 */
typedef struct {
    int sign; /* -1, 0, +1; 0 is represented by sign=0,len=1,d[0]=0 */
    int len;
    unsigned char d[EXAM_BIGINT_CAP]; /* least significant digit first */
} BigInt;
/**
 * @public
 * @requires BigInt exam_algo_fail
 */
/**
 * @id bigint_create
 * @title 大整数构造与初始化
 * @members BigInt bigint_zero bigint_from_str bigint_from_ll
 * @related bigint_io bigint_arithmetic
 * BigInt 是固定容量的有符号十进制整数，默认最多 2048 位。先声明变量，再通过 bigint_zero、bigint_from_str 或 bigint_from_ll 初始化；不需要 free。
 * @example
 * BigInt a, b;
 * bigint_from_str(&a, "12345678901234567890");
 * bigint_from_ll(&b, -15);
 */
static inline void bigint_zero(BigInt *out) {
    if (!out) exam_algo_fail("null BigInt destination");
    memset(out, 0, sizeof(*out));
    out->len = 1;
}
/** @requires BigInt */
static inline void exam_bigint_normalize(BigInt *a) {
    while (a->len > 1 && !a->d[a->len-1]) --a->len;
    if (a->len == 1 && !a->d[0]) a->sign = 0;
    else if (!a->sign) a->sign = 1;
}
/** @requires BigInt */
static inline int exam_bigint_cmp_abs(const BigInt *a, const BigInt *b) {
    if (a->len != b->len) return (a->len > b->len) ? 1 : -1;
    for (int i = a->len - 1; i >= 0; --i)
        if (a->d[i] != b->d[i]) return (a->d[i] > b->d[i]) ? 1 : -1;
    return 0;
}
/* Neither destination may alias operands for these PRIVATE helpers. */
/** @requires BigInt bigint_zero EXAM_BIGINT_CAP exam_algo_fail exam_bigint_normalize */
static inline void exam_bigint_add_abs(BigInt *out, const BigInt *a,
                                       const BigInt *b) {
    bigint_zero(out);
    int n = a->len > b->len ? a->len : b->len;
    int carry = 0;
    for (int i = 0; i < n; ++i) {
        int value = (i < a->len ? a->d[i] : 0) +
                    (i < b->len ? b->d[i] : 0) + carry;
        out->d[i] = (unsigned char)(value % 10);
        carry = value / 10;
    }
    out->len = n;
    if (carry) {
        if (n >= EXAM_BIGINT_CAP) exam_algo_fail("BigInt addition overflow");
        out->d[out->len++] = (unsigned char)carry;
    }
    out->sign = 1;
    exam_bigint_normalize(out);
}
/** @requires BigInt bigint_zero exam_bigint_normalize */
static inline void exam_bigint_sub_abs(BigInt *out, const BigInt *a,
                                       const BigInt *b) {
    bigint_zero(out);
    out->len = a->len;
    int borrow = 0;
    for (int i = 0; i < a->len; ++i) {
        int value = a->d[i] - (i < b->len ? b->d[i] : 0) - borrow;
        borrow = value < 0;
        if (borrow) value += 10;
        out->d[i] = (unsigned char)value;
    }
    out->sign = 1;
    exam_bigint_normalize(out);
}
/** @requires BigInt exam_bigint_normalize */
static inline void exam_bigint_sub_abs_inplace(BigInt *a, const BigInt *b) {
    int borrow = 0;
    for (int i = 0; i < a->len; ++i) {
        int value = a->d[i] - (i < b->len ? b->d[i] : 0) - borrow;
        borrow = value < 0;
        if (borrow) value += 10;
        a->d[i] = (unsigned char)value;
    }
    exam_bigint_normalize(a);
}
