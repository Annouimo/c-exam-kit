
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
 * @id frequency_ints
 * @title 统计频数
 * @tags 数组 整数 统计
 * @summary 统计指定范围内每个非负整数的出现次数。
 * @pre 元素在 0..max_value 范围内，freq 至少 max_value+1 项。
 * @keywords 计数 频率 桶
 * @members frequency_ints frequency_ints_into
 * 统计整数 0..max_value 出现的次数，freq 至少 max_value+1 项；函数初始化计数数组。
 * @example
 * int a[] = {1, 2, 1, 3}, freq[4];
 * frequency_ints(a, 4, freq, 3); // 0, 2, 1, 1
 */
#define frequency_ints(values,n,frequency,max_value) \
    frequency_ints_into((values),(n),(frequency),(max_value), \
                         sizeof(frequency)/sizeof((frequency)[0]))
