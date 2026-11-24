/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero bigint_mul_small
 */
static inline void bigint_factorial(BigInt *out, int n) {
    if (!out || n < 0) exam_algo_fail("invalid BigInt factorial");
    BigInt result;
    bigint_zero(&result);
    result.d[0] = 1;
    result.sign = 1;
    for (int i = 2; i <= n; ++i)
        bigint_mul_small(&result, &result, i);
    *out = result;
}
