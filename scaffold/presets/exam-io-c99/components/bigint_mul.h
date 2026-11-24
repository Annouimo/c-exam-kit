/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero EXAM_BIGINT_CAP exam_bigint_normalize
 */
static inline void bigint_mul(BigInt *out, const BigInt *a,
                               const BigInt *b) {
    if (!out || !a || !b) exam_algo_fail("null BigInt multiplication operand");
    BigInt result;
    bigint_zero(&result);
    if (!a->sign || !b->sign) { *out = result; return; }
    if (a->len + b->len - 1 > EXAM_BIGINT_CAP)
        exam_algo_fail("BigInt multiplication overflow");
    for (int i = 0; i < a->len; ++i) {
        int carry = 0, j = 0;
        for (; j < b->len || carry; ++j) {
            int position = i+j;
            if (position >= EXAM_BIGINT_CAP) exam_algo_fail("BigInt multiplication overflow");
            int value = result.d[position] + carry +
                (j < b->len ? a->d[i] * b->d[j] : 0);
            result.d[position] = (unsigned char)(value % 10);
            carry = value / 10;
        }
    }
    int size = a->len+b->len;
    result.len = size <= EXAM_BIGINT_CAP ? size : EXAM_BIGINT_CAP;
    result.sign = a->sign*b->sign;
    exam_bigint_normalize(&result);
    *out = result;
}
