/** @category 数组 */
/** @requires exam_check_n exam_algo_fail */
static inline void exam_doubles_finite(const double *a,int n) {
    exam_check_n(n);
    for(int i=0;i<n;++i)if(!isfinite(a[i]))
        exam_algo_fail("double array contains nonfinite value");
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void fill_doubles_into(double *a,int n,double value,
                                     size_t capacity) {
    if(n<0 || (size_t)n>capacity)
        exam_algo_fail("fill_doubles exceeds capacity");
    for(int i=0;i<n;++i)a[i]=value;
}
/**
 * @public
 * @requires exam_algo_fail
 */
static inline void copy_doubles_into(double *dst,const double *src,int n,
                                     size_t capacity) {
    if(n<0 || (size_t)n>capacity)
        exam_algo_fail("copy_doubles exceeds capacity");
    if(n)memmove(dst,src,(size_t)n*sizeof(double));
}
static inline int exam_cmp_double(const void *pa,const void *pb) {
    double a=*(const double *)pa,b=*(const double *)pb;
    return (a>b)-(a<b);
}
/**
 * @public
 * @requires exam_doubles_finite exam_cmp_double
 */
static inline void sort_doubles(double *a,int n) {
    exam_doubles_finite(a,n);
    if(n>1)qsort(a,(size_t)n,sizeof(*a),exam_cmp_double);
}
/**
 * @public
 * @requires exam_check_n
 */
static inline void reverse_doubles(double *a,int n) {
    exam_check_n(n);
    for(int i=0;i<n/2;++i) {
        double t=a[i];a[i]=a[n-1-i];a[n-1-i]=t;
    }
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int find_doubles(const double *a,int n,double key) {
    exam_check_n(n);
    for(int i=0;i<n;++i)if(a[i]==key)return i;
    return -1;
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int lower_bound_doubles(const double *a,int n,double key) {
    exam_check_n(n);
    int l=0,r=n;
    while(l<r) {
        int mid=l+(r-l)/2;
        if(a[mid]<key)l=mid+1;else r=mid;
    }
    return l;
}
/**
 * @public
 * @requires exam_check_n
 */
static inline int upper_bound_doubles(const double *a,int n,double key) {
    exam_check_n(n);
    int l=0,r=n;
    while(l<r) {
        int mid=l+(r-l)/2;
        if(a[mid]<=key)l=mid+1;else r=mid;
    }
    return l;
}
/**
 * @public
 * @requires exam_doubles_finite exam_algo_fail
 */
static inline double sum_doubles(const double *a,int n) {
    exam_doubles_finite(a,n);
    long double total=0;
    for(int i=0;i<n;++i)total+=(long double)a[i];
    if(fabsl(total)>DBL_MAX)exam_algo_fail("sum_doubles overflow");
    return (double)total;
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite
 */
static inline double mean_doubles(const double *a,int n) {
    if(n<=0)exam_algo_fail("mean_doubles needs nonempty input");
    exam_doubles_finite(a,n);
    long double total=0;
    for(int i=0;i<n;++i)total+=(long double)a[i];
    return (double)(total/n);
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite
 */
static inline double min_doubles(const double *a,int n) {
    if(n<=0)exam_algo_fail("min_doubles needs nonempty input");
    exam_doubles_finite(a,n);
    double value=a[0];
    for(int i=1;i<n;++i)if(a[i]<value)value=a[i];
    return value;
}
/**
 * @public
 * @requires exam_algo_fail exam_doubles_finite
 */
static inline double max_doubles(const double *a,int n) {
    if(n<=0)exam_algo_fail("max_doubles needs nonempty input");
    exam_doubles_finite(a,n);
    double value=a[0];
    for(int i=1;i<n;++i)if(a[i]>value)value=a[i];
    return value;
}
/**
 * @public
 * @requires fill_doubles_into
 */
#define fill_doubles(a,n,value) fill_doubles_into((a),(n),(value),sizeof(a)/sizeof((a)[0]))
/**
 * @public
 * @requires copy_doubles_into
 */
#define copy_doubles(a,src,n) copy_doubles_into((a),(src),(n),sizeof(a)/sizeof((a)[0]))
