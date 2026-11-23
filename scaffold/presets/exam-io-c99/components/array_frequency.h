/* Count nonnegative values in [0,max_value] using user-owned storage. */
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
#define frequency_ints(values,n,frequency,max_value) \
    frequency_ints_into((values),(n),(frequency),(max_value), \
                         sizeof(frequency)/sizeof((frequency)[0]))
