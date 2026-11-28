/** @category 数学、数位与二分 */
/**
 * @public
 * @requires exam_algo_fail
 * @category 数学、数位与二分
 */
/**
 * @id nearly_equal
 * @title 浮点数比较
 * @members nearly_equal
 * 用绝对误差和相对误差容限判断两个数是否近似相等；容限为非负数。
 * @example
 * bool same = nearly_equal(0.1 + 0.2, 0.3, 1e-9, 1e-9);
 */
static inline bool nearly_equal(double a, double b,
                                double absolute_tolerance,
                                double relative_tolerance) {
    if (!isfinite(a) || !isfinite(b) || absolute_tolerance < 0 ||
        relative_tolerance < 0 || !isfinite(absolute_tolerance) ||
        !isfinite(relative_tolerance))
        exam_algo_fail("invalid floating comparison arguments");
    double delta = fabs(a - b);
    double scale = fmax(fabs(a), fabs(b));
    return delta <= fmax(absolute_tolerance, relative_tolerance * scale);
}
/**
 * @public
 * @requires exam_algo_fail
 */
/**
 * @id round_digits
 * @title 四舍五入
 * @members round_digits
 * 将数舍入到指定小数位数，返回 double。输出固定小数位仍需 printf 或 print_double。
 * @example
 * double x = round_digits(3.14159, 2); // 3.14
 */
static inline double round_digits(double value, int digits) {
    if (!isfinite(value) || digits < -15 || digits > 15)
        exam_algo_fail("invalid round_digits argument");
    double scale = pow(10.0, (double)digits);
    double scaled = value * scale;
    if (!isfinite(scaled)) exam_algo_fail("round_digits overflow");
    return round(scaled) / scale;
}
