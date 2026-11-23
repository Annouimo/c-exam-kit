static inline void fill_lls_into(long long *a, int n, long long value,
                                  size_t capacity) {
    if (n<0 || (size_t)n>capacity) exam_algo_fail("fill_lls exceeds capacity");
    for(int i=0;i<n;++i)a[i]=value;
}
static inline void copy_lls_into(long long *dest,const long long *src,int n,
                                  size_t capacity) {
    if(n<0 || (size_t)n>capacity)exam_algo_fail("copy_lls exceeds capacity");
    if(n)memmove(dest,src,(size_t)n*sizeof(long long));
}
static inline int exam_cmp_ll(const void *pa,const void *pb) {
    long long a=*(const long long *)pa,b=*(const long long *)pb;
    return (a>b)-(a<b);
}
static inline void sort_lls(long long *a,int n) {
    exam_check_n(n);
    if(n>1)qsort(a,(size_t)n,sizeof(*a),exam_cmp_ll);
}
static inline void reverse_lls(long long *a,int n) {
    exam_check_n(n);
    for(int i=0;i<n/2;++i) {
        long long t=a[i];a[i]=a[n-1-i];a[n-1-i]=t;
    }
}
static inline int find_lls(const long long *a,int n,long long key) {
    exam_check_n(n);
    for(int i=0;i<n;++i)if(a[i]==key)return i;
    return -1;
}
static inline int lower_bound_lls(const long long *a,int n,long long key) {
    exam_check_n(n);
    int l=0,r=n;
    while(l<r) {
        int mid=l+(r-l)/2;
        if(a[mid]<key)l=mid+1;else r=mid;
    }
    return l;
}
static inline int upper_bound_lls(const long long *a,int n,long long key) {
    exam_check_n(n);
    int l=0,r=n;
    while(l<r) {
        int mid=l+(r-l)/2;
        if(a[mid]<=key)l=mid+1;else r=mid;
    }
    return l;
}
static inline long long sum_lls(const long long *a,int n) {
    exam_check_n(n);
    long long total=0;
    for(int i=0;i<n;++i) {
        if((a[i]>0 && total>LLONG_MAX-a[i]) ||
           (a[i]<0 && total<LLONG_MIN-a[i]))
            exam_algo_fail("sum_lls overflow");
        total+=a[i];
    }
    return total;
}
static inline long long min_lls(const long long *a,int n) {
    if(n<=0)exam_algo_fail("min_lls needs nonempty input");
    long long value=a[0];
    for(int i=1;i<n;++i)if(a[i]<value)value=a[i];
    return value;
}
static inline long long max_lls(const long long *a,int n) {
    if(n<=0)exam_algo_fail("max_lls needs nonempty input");
    long long value=a[0];
    for(int i=1;i<n;++i)if(a[i]>value)value=a[i];
    return value;
}
#define fill_lls(a,n,value) fill_lls_into((a),(n),(value),sizeof(a)/sizeof((a)[0]))
#define copy_lls(a,src,n) copy_lls_into((a),(src),(n),sizeof(a)/sizeof((a)[0]))
