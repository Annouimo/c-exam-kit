/** @category 数学、数位与二分 */
/* Bisection on [left,right]; continuous f with opposite endpoint signs. */
/**
 * @public
 * @requires exam_algo_fail
 * @category 数学、数位与二分
 * @title 二分求根
 * @members bisect_root
 * `bisect_root(f, l, r, tol, max_iter)` 返回近似根，`f` 是 `double f(double x)`。取连续函数的异号区间，端点恰好为根也可以
 */
static inline double bisect_root(double (*f)(double), double left,
                                 double right, double tolerance, int iterations) {
    if(!f || !isfinite(left) || !isfinite(right) || left>=right ||
       !isfinite(tolerance) || tolerance<=0 || iterations<=0)
        exam_algo_fail("invalid bisect_root arguments");
    double a=f(left), b=f(right);
    if(!isfinite(a) || !isfinite(b))
        exam_algo_fail("nonfinite endpoint function value");
    if(a==0) return left;
    if(b==0) return right;
    if((a<0)==(b<0))
        exam_algo_fail("bisect_root requires opposite endpoint signs");
    for(int i=0;i<iterations && right-left>tolerance;++i) {
        double mid=left+(right-left)/2;
        if(mid==left || mid==right)break;
        double value=f(mid);
        if(!isfinite(value))exam_algo_fail("nonfinite function value");
        if(value==0)return mid;
        if((value<0)==(a<0)) { left=mid; a=value; }
        else { right=mid; b=value; }
    }
    return left+(right-left)/2;
}
