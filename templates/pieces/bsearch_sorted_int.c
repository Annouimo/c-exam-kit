int bsearch_sorted_int(const int a[], int n, int key) {
    const int *found = (const int *)bsearch(&key, a, (size_t)n, sizeof a[0], cmp_int);
    return found ? (int)(found - a) : -1;
}
