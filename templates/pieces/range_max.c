int range_max(int left, int right) {
    int k = log2_floor[right - left + 1];
    int x = st[k][left], y = st[k][right - (1 << k) + 1];
    return x > y ? x : y;
}
