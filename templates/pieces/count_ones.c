int count_ones(unsigned long long x) {
    int count = 0;
    while (x) {
        x &= x - 1;                @COMMENT:count_ones_0@
        count++;
    }
    return count;
}
