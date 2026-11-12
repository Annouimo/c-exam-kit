long long digits_to_int(int a[], int len, int base) {
    long long value = 0;
    for (int i = 0; i < len; i++) value = value * base + a[i];
    return value;
}
