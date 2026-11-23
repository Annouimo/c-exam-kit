static inline int linear_search_ints(const int *a, int n, int key) {
    exam_check_n(n);
    for (int i = 0; i < n; ++i) if (a[i] == key) return i;
    return -1;
}
static inline int find_ints(const int *a, int n, int key) {
    return linear_search_ints(a, n, key);
}
static inline int lower_bound_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right-left)/2;
        if (a[mid] < key) left = mid+1;
        else right = mid;
    }
    return left;
}
static inline int upper_bound_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int left = 0, right = n;
    while (left < right) {
        int mid = left + (right-left)/2;
        if (a[mid] <= key) left = mid+1;
        else right = mid;
    }
    return left;
}
static inline int binary_search_ints(const int *a, int n, int key) {
    int i = lower_bound_ints(a, n, key);
    return i < n && a[i] == key ? i : -1;
}
static inline int count_ints(const int *a, int n, int key) {
    exam_check_n(n);
    int count = 0;
    for (int i = 0; i < n; ++i) if (a[i] == key) ++count;
    return count;
}
