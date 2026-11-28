/** @category 数学、数位与二分 */
/* Coefficients from constant term to highest degree.
 * All intermediate signed operations are checked. */
/**
 * @public
 * @requires exam_algo_fail exam_mul_ll exam_add_checked_ll
 */
/**
 * @id poly_eval_lls
 * @title 多项式求值
 * @members poly_eval_lls
 * coefficients 按低次到高次排列，count 是系数个数，coefficients[0] 为常数项。返回 long long。
 * @example
 * long long coef[] = {1, 2, 3};
 * long long y = poly_eval_lls(coef, 3, 2); // 17
 */
static inline long long poly_eval_lls(const long long *coefficients,
                                      int count, long long x) {
    if(count<=0)exam_algo_fail("poly_eval_lls needs at least one coefficient");
    long long result=coefficients[count-1];
    for(int i=count-2;i>=0;--i) {
        long long product=exam_mul_ll(result,x);
        result=exam_add_checked_ll(product,coefficients[i]);
    }
    return result;
}
