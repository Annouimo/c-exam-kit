/* 闭区间 [left,right]，要求 1<=left<=right<=n */
long long range_sum(long long prefix[], int left, int right) {
    return prefix[right] - prefix[left - 1];
}
