int big_sub(BigInt a, BigInt b, BigInt *out) {
    int sign = big_cmp(a, b);
    if (sign >= 0) *out = big_sub_abs(a, b);
    else *out = big_sub_abs(b, a);
    return sign;
}
