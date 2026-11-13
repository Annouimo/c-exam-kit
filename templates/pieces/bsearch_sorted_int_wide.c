int bsearch_sorted_int(const long long a[], int n, long long key) {
    const long long *found = (const long long *)bsearch(&key, a, (size_t)n, sizeof a[0], cmp_int);
    return found ? (int)(found - a) : -1;
}
