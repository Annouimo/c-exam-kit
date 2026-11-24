/** @category 数组 */
/* Count nonnegative values in [0,max_value] using user-owned storage. */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void frequency_ints_into(const int *values, int n,
                                       int *frequency, int max_value,
                                       size_t capacity) {
    if(n<0 || max_value<0 || (size_t)max_value>=capacity)
        exam_algo_fail("frequency_ints exceeds destination capacity");
    for(int x=0;x<=max_value;++x)frequency[x]=0;
    for(int i=0;i<n;++i) {
        int x=values[i];
        if(x<0 || x>max_value)
            exam_algo_fail("frequency_ints input outside nonnegative range");
        if(frequency[x]==INT_MAX)
            exam_algo_fail("frequency_ints counter overflow");
        ++frequency[x];
    }
}
/**
 * @public
 * @requires frequency_ints_into
 * @category 数组
 * @title 频数、排序后的下标
 * @members frequency_ints argsort_ints
 * `frequency_ints(a, n, freq, max_value)` 统计 `0..max_value`，`freq` 至少有 `max_value + 1` 项。`argsort_ints(a, n, order)` 把升序所对应的原下标写入 `order`，相等项保持原顺序
 */
#define frequency_ints(values,n,frequency,max_value) \
    frequency_ints_into((values),(n),(frequency),(max_value), \
                         sizeof(frequency)/sizeof((frequency)[0]))
