BigInt big_mul_small(BigInt a, int b) {
    BigInt c = big_zero();
    c.len = a.len;
    long long carry = 0;
    for (int i = 0; i < a.len; i++) {
        long long value = (long long)a.d[i] * b + carry;
        c.d[i] = (int)(value % 10);
        carry = value / 10;
    }
    while (carry) {
        c.d[c.len++] = (int)(carry % 10);
        carry /= 10;
    }
    while (c.len > 1 && c.d[c.len - 1] == 0) c.len--;
    return c;
}
