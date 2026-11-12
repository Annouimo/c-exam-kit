long long max_subarray_sum(int a[], int n) {
    long long ending = a[0], best = a[0];
    for (int i = 1; i < n; i++) {
        if (ending > 0) ending += a[i];
        else ending = a[i];
        if (ending > best) best = ending;
    }
    return best;
}
