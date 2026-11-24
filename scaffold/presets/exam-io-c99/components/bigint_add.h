/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires BigInt exam_algo_fail exam_bigint_add_abs exam_bigint_cmp_abs bigint_zero exam_bigint_sub_abs
 * @category 大整数、枚举及其他类型
 */
/**
 * @id bigint_arithmetic
 * @title 大整数加减法
 * @members bigint_add bigint_sub
 * @related bigint_mul bigint_div
 * 结果写入第一个参数，允许结果与操作数别名。结果超出容量时报告错误。
 * @example
 * BigInt a, b, c;
 * bigint_from_str(&a, "12345678901234567890");
 * bigint_from_ll(&b, -15);
 * bigint_add(&c, &a, &b);
 * bigint_print(&c, "\n");
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
