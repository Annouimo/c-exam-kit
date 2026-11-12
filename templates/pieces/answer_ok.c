int answer_ok(long long x) {
    /* TODO: 改成从假到真的单调条件；此处仅为示例 */
    return x >= 10;
}
long long first_true(long long left, long long right) {
    /* 要求闭区间内至少一个真值，left<=right；right-left不溢出 */
    while (left < right) {
        long long mid = left + (right-left)/2;
        if (answer_ok(mid)) right = mid;
        else left = mid+1;
    }
    return left;
}
