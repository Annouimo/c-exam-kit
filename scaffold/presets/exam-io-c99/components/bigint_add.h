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
static inline void bigint_sub(BigInt *out, const BigInt *a,
                               const BigInt *b) {
    if (!out || !a || !b) exam_algo_fail("null BigInt subtraction operand");
    BigInt inverted = *b;
    inverted.sign = -inverted.sign;
    bigint_add(out, a, &inverted);
}
