void big_divmod(BigInt a, BigInt b, BigInt *q, BigInt *rem) {
    *q = big_zero();
    *rem = big_zero();
    q->len = a.len;
    for (int i = a.len - 1; i >= 0; i--) {
        /* 余数乘 10，再带下当前数位 */
        for (int j = rem->len; j > 0; j--) rem->d[j] = rem->d[j-1];
        rem->d[0] = a.d[i];
        rem->len++;
        while (rem->len > 1 && rem->d[rem->len-1] == 0) rem->len--;
        while (big_cmp(*rem, b) >= 0) {
            *rem = big_sub_abs(*rem, b);
            q->d[i]++;
        }
    }
    while (q->len > 1 && q->d[q->len-1] == 0) q->len--;
}
