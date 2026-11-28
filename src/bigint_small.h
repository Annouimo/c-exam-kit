/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero EXAM_BIGINT_CAP exam_bigint_normalize
 */
static inline void bigint_mul_small(BigInt *out, const BigInt *a, int factor) {
    if (!out || !a) exam_algo_fail("null BigInt multiplication operand");
    if (factor == 0 || !a->sign) { bigint_zero(out); return; }
    unsigned long long x = factor < 0 ? 0ULL-(unsigned)factor : (unsigned)factor;
    BigInt result;
    bigint_zero(&result);
    unsigned long long carry = 0;
    int i = 0;
    for (; i < a->len || carry; ++i) {
        if (i >= EXAM_BIGINT_CAP) exam_algo_fail("BigInt multiplication overflow");
        unsigned long long value = carry +
            (i < a->len ? (unsigned long long)a->d[i]*x : 0ULL);
        result.d[i] = (unsigned char)(value%10ULL);
        carry = value/10ULL;
    }
    result.len = i;
    result.sign = a->sign * (factor < 0 ? -1 : 1);
    exam_bigint_normalize(&result);
    *out = result;
}
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero exam_bigint_normalize
 */
static inline void bigint_div_small(BigInt *quotient, int *remainder,
                                    const BigInt *a, int divisor) {
    if (!quotient || !remainder || !a || divisor == 0)
        exam_algo_fail("invalid BigInt small division arguments");
    unsigned long long x = divisor < 0 ? 0ULL-(unsigned)divisor : (unsigned)divisor;
    BigInt result;
    bigint_zero(&result);
    result.len = a->len;
    unsigned long long carry = 0;
    for (int i = a->len-1; i >= 0; --i) {
        unsigned long long current = carry*10ULL + a->d[i];
        result.d[i] = (unsigned char)(current/x);
        carry = current%x;
    }
    result.sign = a->sign*(divisor < 0 ? -1 : 1);
    exam_bigint_normalize(&result);
    *quotient = result;
    *remainder = (int)carry*(a->sign < 0 ? -1 : 1);
}
