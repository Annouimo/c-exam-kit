/* 非负整数拆成高位在前的数位数组；n=0 得到一位 0 */
int int_to_digits(long long n, int base, int a[]) {
    int len = 0;
    do {
        a[len++] = (int)(n % base);
        n /= base;
    } while (n > 0);
    for (int i = 0; i < len / 2; i++) {
        int t = a[i]; a[i] = a[len - 1 - i]; a[len - 1 - i] = t;
    }
    return len;
}
