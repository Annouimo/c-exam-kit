int answer_ok(long long x) {
    @COMMENT:answer_ok_0@
    return x >= 10;
}
long long first_true(long long left, long long right) {
    @COMMENT:answer_ok_1@
    while (left < right) {
        long long mid = left + (right-left)/2;
        if (answer_ok(mid)) right = mid;
        else left = mid+1;
    }
    return left;
}
