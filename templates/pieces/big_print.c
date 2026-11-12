void big_print(BigInt a) {
    for (int i = a.len - 1; i >= 0; i--) printf("%d", a.d[i]);
}
