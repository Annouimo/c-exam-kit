BigInt big_div_small(BigInt a, int b, int *remainder) {
    BigInt q = big_zero();
    q.len = a.len;
    long long rem = 0;
    for (int i = a.len - 1; i >= 0; i--) {
        rem = rem * 10 + a.d[i];
        q.d[i] = (int)(rem / b);
        rem %= b;
    }
    while (q.len > 1 && q.d[q.len - 1] == 0) q.len--;
    *remainder = (int)rem;
    return q;
}
