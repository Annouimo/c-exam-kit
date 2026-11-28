
/* Bisection on [left,right]; continuous f with opposite endpoint signs. */
/**
 * @public
 * @requires exam_algo_fail
 * @id bisect_root
 * @title 二分求根
 * @tags 浮点数 数学计算 二分
 * @summary 在连续函数的异号区间内二分求根。
 * @pre 连续函数在区间端点异号或端点为根，误差容限为正。
 * @keywords 零点 方程 求根
 * @members bisect_root
 * 在连续函数的异号区间 [left,right] 内二分求根，端点为根也可。tolerance 为正的误差容限，iterations 为最大迭代次数。
 * @example
 * static double f(double x) { return x*x - 2; }
 * int main(void) {
 *     double x = bisect_root(f, 1, 2, 1e-8, 100);
 *     print_double(x, 6, "\n");
 *     return 0;
 * }
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
