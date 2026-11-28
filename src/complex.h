/**
 * @public
 */
typedef struct {
    double re, im;
} Complex;
/**
 * @public
 * @requires Complex
 * @id complex_create
 * @title 构造复数
 * @tags 复数 构造
 * @summary 构造含实部和虚部的复数。
 * @keywords 实部 虚部 complex
 * @members Complex complex_new
 * @related complex_arithmetic complex_output
 * Complex 是包含实部 re 和虚部 im 的值类型。complex_new(real, imaginary) 构造复数。
 * @example
 * Complex z = complex_new(3, 4);
 */
static inline Complex complex_new(double real, double imaginary) {
    Complex z = {real, imaginary};
    return z;
}
/**
 * @public
 * @requires Complex complex_new
 * @id complex_arithmetic
 * @title 复数四则运算
 * @tags 复数 数学计算
 * @summary 对两个复数进行四则运算。
 * @pre 除数不能为零。
 * @keywords 加减乘除 四则
 * @members complex_add complex_sub complex_mul complex_div
 * 接收两个 Complex 值，返回运算结果，不修改原值。除数不能为零。
 * @example
 * Complex a = complex_new(1, 2), b = complex_new(3, 4);
 * Complex c = complex_add(a, b);
 * Complex d = complex_mul(a, b);
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
 * @id complex_output
 * @title 复数模长与输出
 * @tags 复数 输出 数学计算
 * @summary 计算复数模长或按指定精度输出复数。
 * @keywords 模长 绝对值 打印
 * @members complex_abs complex_print
 * complex_abs 返回模；complex_print 指定小数位数 digits 和结束字符串 end。
 * @example
 * Complex z = complex_new(3, 4);
 * double m = complex_abs(z); // 5
 * complex_print(z, 2, "\n");
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
