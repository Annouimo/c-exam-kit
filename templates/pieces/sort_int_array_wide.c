void sort_int_array(long long a[], int n) {
    qsort(a, (size_t)n, sizeof a[0], cmp_int);
}
