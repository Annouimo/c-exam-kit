int longest_run(long long a[], int n, int consecutive) {
    if (n == 0) return 0;
    int length = 1, best = 1;
    for (int i = 1; i < n; i++) {
        int ok = consecutive ? (a[i] > a[i-1] && (unsigned long long)a[i] - (unsigned long long)a[i-1] == 1) : a[i] == a[i-1];
        length = ok ? length+1 : 1;
        if (length > best) best = length;
    }
    return best;
}

/* 指定值最长连续段；没有匹配元素时返回 0。 */
int longest_value_run(const long long a[], int n, long long target) {
    int best = 0, length = 0;
    for (int i = 0; i < n; ++i) {
        length = (a[i] == target) ? length + 1 : 0;
        if (length > best) best = length;
    }
    return best;
}
