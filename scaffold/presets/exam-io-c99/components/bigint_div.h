/* Quotient truncates toward zero; remainder keeps dividend sign. */
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
