BigInt big_add(BigInt a, BigInt b) {
    BigInt c = big_zero();
    c.len = a.len > b.len ? a.len : b.len;
    int carry = 0;
    for (int i = 0; i < c.len; i++) {
        int sum = a.d[i] + b.d[i] + carry;
        c.d[i] = sum % 10;
        carry = sum / 10;
    }
    if (carry) c.d[c.len++] = carry;
    return c;
}
