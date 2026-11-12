BigInt big_mul(BigInt a, BigInt b) {
    BigInt c = big_zero();
    if ((a.len == 1 && a.d[0] == 0) || (b.len == 1 && b.d[0] == 0)) return c;
    c.len = a.len + b.len;
    for (int i = 0; i < a.len; i++) {
        int carry = 0;
        for (int j = 0; j < b.len; j++) {
            int value = c.d[i+j] + a.d[i]*b.d[j] + carry;
            c.d[i+j] = value % 10;
            carry = value / 10;
        }
        c.d[i+b.len] = carry;
    }
    while (c.len > 1 && c.d[c.len - 1] == 0) c.len--;
    return c;
}
