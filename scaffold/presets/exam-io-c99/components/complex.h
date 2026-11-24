/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @category 大整数、枚举及其他类型
 * @title 复数
 * @members Complex complex_add complex_sub complex_mul complex_div complex_abs complex_print
 * `Complex z = complex_new(real, imag)`。`complex_add(a, b)`、`complex_sub(a, b)`、`complex_mul(a, b)`、`complex_div(a, b)` 返回复数。`complex_abs(z)` 返回模，`complex_print(z, digits, end)` 输出
 */
typedef struct {
    double re, im;
} Complex;
/**
 * @public
 * @requires Complex
 */
static inline Complex complex_new(double real, double imaginary) {
    Complex z = {real, imaginary};
    return z;
}
/**
 * @public
 * @requires Complex complex_new
 */
static inline Complex complex_add(Complex a, Complex b) {
    return complex_new(a.re + b.re, a.im + b.im);
}
/**
 * @public
 * @requires Complex complex_new
 */
static inline Complex complex_sub(Complex a, Complex b) {
    return complex_new(a.re - b.re, a.im - b.im);
}
/**
 * @public
 * @requires Complex complex_new
 */
static inline Complex complex_mul(Complex a, Complex b) {
    return complex_new(a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re);
}
/**
 * @public
 * @requires Complex exam_algo_fail complex_new
 */
static inline Complex complex_div(Complex a, Complex b) {
    if (b.re == 0 && b.im == 0) exam_algo_fail("complex division by zero");
    if (fabs(b.re) >= fabs(b.im)) {
        double r = b.im / b.re;
        double den = b.re + b.im * r;
        return complex_new((a.re + a.im*r)/den, (a.im - a.re*r)/den);
    }
    double r = b.re / b.im;
    double den = b.im + b.re * r;
    return complex_new((a.re*r + a.im)/den, (a.im*r - a.re)/den);
}
/**
 * @public
 * @requires Complex
 */
static inline double complex_abs(Complex z) {
    return hypot(z.re, z.im);
}
/**
 * @public
 * @requires Complex exam_algo_fail
 */
static inline void complex_print(Complex z, int digits, const char *end) {
    if (digits < 0 || digits > 100) exam_algo_fail("invalid precision");
    if (printf("%.*f %.*f%s", digits, z.re, digits, z.im, end) < 0)
        exam_algo_fail("complex output failed");
}
