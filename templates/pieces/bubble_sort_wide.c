void bubble_sort(long long a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                swap_int(&a[j], &a[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}
