long long max_forward_diff(long long a[], int n) {
    long long max_before = a[0];
    long long best = (long long)a[0] - a[1];
    for (int j = 1; j < n; j++) {
        long long diff = max_before - a[j];
        if (diff > best) best = diff;
        if (a[j] > max_before) max_before = a[j];
    }
    return best;
}
