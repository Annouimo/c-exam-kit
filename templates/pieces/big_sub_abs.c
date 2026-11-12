BigInt big_sub_abs(BigInt a, BigInt b) {
    BigInt c = big_zero();
    c.len = a.len;
    int borrow = 0;
    for (int i = 0; i < a.len; i++) {
        int value = a.d[i] - b.d[i] - borrow;
        borrow = value < 0;
        if (borrow) value += 10;
        c.d[i] = value;
    }
    while (c.len > 1 && c.d[c.len - 1] == 0) c.len--;
    return c;
}
