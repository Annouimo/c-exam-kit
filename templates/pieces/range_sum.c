@COMMENT:range_sum_0@
long long range_sum(long long prefix[], int left, int right) {
    return prefix[right] - prefix[left - 1];
}
