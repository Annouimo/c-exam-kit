/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail exam_bigint_add_abs exam_bigint_cmp_abs bigint_zero exam_bigint_sub_abs
 * @category 大整数、枚举及其他类型
 * @title 大整数计算
 * @members bigint_add bigint_sub bigint_mul bigint_divmod bigint_compare bigint_factorial
 * `bigint_add(&c, &a, &b)`、`bigint_sub(&c, &a, &b)`、`bigint_mul(&c, &a, &b)` 均把结果写入第一个参数。`bigint_divmod(&q, &r, &a, &b)` 求商和余数，商向零截断。`bigint_compare(&a, &b)` 返回 `-1`、`0` 或 `1`。`bigint_factorial(&c, n)` 求阶乘
 */
static inline void bigint_add(BigInt *out, const BigInt *a,
                               const BigInt *b) {
    if (!out || !a || !b) exam_algo_fail("null BigInt addition operand");
    BigInt result;
    if (!a->sign) result = *b;
    else if (!b->sign) result = *a;
    else if (a->sign == b->sign) {
        exam_bigint_add_abs(&result, a, b);
        if (result.sign) result.sign = a->sign;
    } else {
        int cmp = exam_bigint_cmp_abs(a, b);
        if (cmp == 0) bigint_zero(&result);
        else if (cmp > 0) {
            exam_bigint_sub_abs(&result, a, b);
            result.sign = a->sign;
        } else {
            exam_bigint_sub_abs(&result, b, a);
            result.sign = b->sign;
        }
    }
    *out = result;
}
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_add
 */
static inline void bigint_sub(BigInt *out, const BigInt *a,
                               const BigInt *b) {
    if (!out || !a || !b) exam_algo_fail("null BigInt subtraction operand");
    BigInt inverted = *b;
    inverted.sign = -inverted.sign;
    bigint_add(out, a, &inverted);
}
