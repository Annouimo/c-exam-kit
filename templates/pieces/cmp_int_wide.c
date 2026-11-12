int cmp_int(const void *a, const void *b) {
    long long x = *(const long long *)a, y = *(const long long *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}
