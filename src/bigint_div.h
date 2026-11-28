
/* Quotient truncates toward zero; remainder keeps dividend sign. */
/**
 * @public
 * @requires BigInt exam_algo_fail bigint_zero EXAM_BIGINT_CAP exam_bigint_normalize exam_bigint_cmp_abs exam_bigint_sub_abs_inplace
 * @id bigint_div
 * @title 大整数除法
 * @tags 大整数 数学计算
 * @summary 计算大整数除法的商与余数。
 * @pre 除数非零，商与余数输出必须是不同变量。
 * @keywords 高精度 除法 取模 商 余数
 * @members bigint_divmod bigint_div_small
 * 除数非零。商向零截断，余数与被除数同号或为零。divmod 的商和余数输出必须是不同变量；div_small 的除数和余数为 int。
 * @example
 * BigInt a, b, q, r;
 * bigint_from_ll(&a, -17);
 * bigint_from_ll(&b, 5);
 * bigint_divmod(&q, &r, &a, &b); // -3, -2
 */
static inline void bigint_divmod(BigInt *quotient, BigInt *remainder,
                                 const BigInt *a, const BigInt *b) {
    if (!quotient || !remainder || !a || !b || quotient == remainder)
        exam_algo_fail("invalid BigInt division arguments");
    if (!b->sign) exam_algo_fail("BigInt division by zero");
    BigInt q, r;
    bigint_zero(&q);
    bigint_zero(&r);
    q.len = a->len;
    for (int i = a->len-1; i >= 0; --i) {
        if (r.len == EXAM_BIGINT_CAP && r.sign)
            exam_algo_fail("BigInt division work buffer overflow");
        if (r.sign) {
            for (int j = r.len; j > 0; --j) r.d[j] = r.d[j-1];
            ++r.len;
        }
        r.d[0] = a->d[i];
        r.sign = 1;
        exam_bigint_normalize(&r);
        int digit = 0;
        while (exam_bigint_cmp_abs(&r, b) >= 0) {
            exam_bigint_sub_abs_inplace(&r, b);
            ++digit;
            if (digit > 9) exam_algo_fail("BigInt division internal error");
        }
        q.d[i] = (unsigned char)digit;
    }
    q.sign = a->sign * b->sign;
    r.sign = a->sign;
    exam_bigint_normalize(&q);
    exam_bigint_normalize(&r);
    *quotient = q;
    *remainder = r;
}
