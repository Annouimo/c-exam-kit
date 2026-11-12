long long base_to_dec(char s[], int base) {
    int i = 0, sign = 1;
    long long value = 0;
    if (s[i] == '-') { sign = -1; i++; }
    for (; s[i]; i++) value = value * base + digit_value(s[i]);
    return sign * value;
}
