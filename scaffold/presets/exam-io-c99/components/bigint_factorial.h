/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero bigint_mul_small
 */
/**
 * @id bigint_factorial
 * @title 大整数阶乘
 * @members bigint_factorial
 * @related factorial_basic
 * 计算非负整数 n 的阶乘，结果写入 BigInt。结果不得超过容量。
 * @example
 * BigInt f;
 * bigint_factorial(&f, 100);
 * bigint_print(&f, "\n");
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
