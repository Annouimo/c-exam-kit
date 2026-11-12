void dec_to_base(long long n, int base, char out[]) {
    char digits[] = "0123456789ABCDEF";
    char temp[65];
    int len = 0;
    do {
        temp[len++] = digits[n % base];
        n /= base;
    } while (n > 0);
    for (int i = 0; i < len; i++) out[i] = temp[len - 1 - i];
    out[len] = '\0';
}
