void build_prefix(int a[], int n, long long prefix[]) {
    prefix[0] = 0;
    for (int i = 1; i <= n; i++) prefix[i] = prefix[i - 1] + a[i];
}
