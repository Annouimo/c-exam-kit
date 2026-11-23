/* Coefficients from constant term to highest degree.
 * All intermediate signed operations are checked. */
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
