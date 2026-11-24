/** @category 数学、数位与二分 */
/**
 * @public
 * @requires exam_algo_fail
 */
static inline int int_to_digits_into(long long value,int base,
                                      int *out,size_t capacity) {
    if(base<2 || base>36)exam_algo_fail("base must be 2..36");
    unsigned long long x=value<0?0ULL-(unsigned long long)value:(unsigned long long)value;
    int reversed[64],n=0;
    do {
        reversed[n++]=(int)(x%(unsigned)base);
        x/=(unsigned)base;
    }while(x);
    if((size_t)n>capacity)exam_algo_fail("digit buffer too small");
    for(int i=0;i<n;++i)out[i]=reversed[n-1-i];
    return n;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline long long digits_to_int(const int *digits,int n,int base) {
    if(base<2 || base>36 || n<=0)exam_algo_fail("invalid digit input");
    unsigned long long value=0;
    for(int i=0;i<n;++i) {
        if(digits[i]<0 || digits[i]>=base)
            exam_algo_fail("digit outside base");
        if(value>((unsigned long long)LLONG_MAX-(unsigned)digits[i])/(unsigned)base)
            exam_algo_fail("digits_to_int overflow");
        value=value*(unsigned)base+(unsigned)digits[i];
    }
    return (long long)value;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline long long int_reverse_digits(long long n) {
    bool negative=n<0;
    unsigned long long x=negative?0ULL-(unsigned long long)n:(unsigned long long)n;
    unsigned long long limit=(unsigned long long)LLONG_MAX+(negative?1ULL:0ULL);
    unsigned long long result=0;
    do {
        unsigned long long digit=x%10ULL;
        if(result>(limit-digit)/10ULL)
            exam_algo_fail("reversed digits out of long long range");
        result=result*10ULL+digit;
        x/=10ULL;
    }while(x);
    if(negative && result==(unsigned long long)LLONG_MAX+1ULL)return LLONG_MIN;
    return negative?-(long long)result:(long long)result;
}
/**
 * @public
 * @requires int_to_digits_into
 * @category 数学、数位与二分
 * @title 拆分与合并数位
 * @members int_to_digits digits_to_int int_reverse_digits
 * `int_to_digits(x, base, a)` 按高位到低位写入并返回位数。`digits_to_int(a, n, base)` 按同一顺序合并。`int_reverse_digits(x)` 反转十进制数位
 */
#define int_to_digits(value,base,array) \
    int_to_digits_into((value),(base),(array),sizeof(array)/sizeof((array)[0]))
