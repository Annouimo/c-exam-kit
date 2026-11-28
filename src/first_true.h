/** @category 数学、数位与二分 */
/* On [begin,end), predicate must be false then true (monotone).
 * Returns end if no integer satisfies it. Supports negative/large bounds.
 */
/**
 * @public
 * @requires exam_algo_fail
 * @category 数学、数位与二分
 */
/**
 * @id first_true_int
 * @title 二分答案
 * @members first_true_int
 * 在 [begin,end) 内寻找第一个满足 predicate 的整数，条件必须从 false 单调变为 true。全不满足返回 end。
 * @example
 * static bool check(int x) { return x >= 10; }
 * int main(void) {
 *     int ans = first_true_int(0, 100, check);
 *     print_int(ans, "\n"); // 10
 *     return 0;
 * }
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
