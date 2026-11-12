int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}
