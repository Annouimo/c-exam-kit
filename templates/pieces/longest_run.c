int longest_run(int a[], int n, int consecutive) {
    if (n == 0) return 0;
    int length = 1, best = 1;
    for (int i = 1; i < n; i++) {
        int ok = consecutive ? (a[i] > a[i-1] && (unsigned long long)a[i] - (unsigned long long)a[i-1] == 1) : a[i] == a[i-1];
        length = ok ? length+1 : 1;
        if (length > best) best = length;
    }
    return best;
}
