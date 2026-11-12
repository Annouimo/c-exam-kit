int big_cmp(BigInt a, BigInt b) {
    if (a.len < b.len) return -1;
    if (a.len > b.len) return 1;
    for (int i = a.len - 1; i >= 0; i--) {
        if (a.d[i] < b.d[i]) return -1;
        if (a.d[i] > b.d[i]) return 1;
    }
    return 0;
}
