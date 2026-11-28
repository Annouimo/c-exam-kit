/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero EXAM_BIGINT_CAP exam_bigint_normalize
 * @id bigint_mul
 * @title 大整数乘法
 * @tags 大整数 数学计算
 * @summary 计算大整数乘积，支持乘以普通整数。
 * @keywords 高精度 乘积 大数
 * @members bigint_mul bigint_mul_small
 * 将乘积写入第一个参数。bigint_mul_small 的第二个操作数是 int；结果可与输入别名。
 * @example
 * BigInt a, b, c;
 * bigint_from_ll(&a, 123);
 * bigint_from_ll(&b, 456);
 * bigint_mul(&c, &a, &b);
 * bigint_mul_small(&c, &c, 2);
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
