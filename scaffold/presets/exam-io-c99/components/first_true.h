/** @category 数学、数位与二分 */
/* On [begin,end), predicate must be false then true (monotone).
 * Returns end if no integer satisfies it. Supports negative/large bounds.
 */
/**
 * @public
 * @requires exam_algo_fail
 * @category 数学、数位与二分
 * @title 二分答案
 * @members first_true_int
 * `first_true_int(l, r, check)` 找 `[l, r)` 中第一个满足 `bool check(int x)` 的整数。条件必须从假单调变真，全不满足时返回 `r`
 */
static inline int first_true_int(int begin, int end,
                                  bool (*predicate)(int)) {
    if (begin > end || !predicate)
        exam_algo_fail("invalid first_true_int interval or predicate");
    int left = begin, right = end;
    while (left < right) {
        int middle = (int)((long long)left +
                           ((long long)right - left) / 2LL);
        if (predicate(middle)) right = middle;
        else left = middle + 1;
    }
    return left;
}
