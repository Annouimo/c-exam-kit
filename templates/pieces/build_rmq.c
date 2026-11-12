void build_rmq(int a[], int n) {
    log2_floor[1] = 0;
    for (int i = 2; i <= n; i++) log2_floor[i] = log2_floor[i/2] + 1;
    for (int i = 0; i < n; i++) st[0][i] = a[i];
    for (int k = 1; (1 << k) <= n; k++) {
        for (int i = 0; i + (1 << k) <= n; i++) {
            int x = st[k-1][i], y = st[k-1][i + (1 << (k-1))];
            st[k][i] = x > y ? x : y;
        }
    }
}
