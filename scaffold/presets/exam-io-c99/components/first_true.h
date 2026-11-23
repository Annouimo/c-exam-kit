/* On [begin,end), predicate must be false then true (monotone).
 * Returns end if no integer satisfies it. Supports negative/large bounds.
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
