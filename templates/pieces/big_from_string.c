BigInt big_from_string(char s[]) {
    BigInt a = big_zero();
    a.len = (int)strlen(s);
    for (int i = 0; i < a.len; i++) a.d[i] = s[a.len - 1 - i] - '0';
    while (a.len > 1 && a.d[a.len - 1] == 0) a.len--;
    return a;
}
