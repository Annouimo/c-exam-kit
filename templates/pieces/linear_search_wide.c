int linear_search(long long a[], int n, long long key) {
    for (int i = 0; i < n; i++)
        if (a[i] == key) return i;
    return -1;
}
