typedef struct {
    double re, im;
} Complex;
static inline Complex complex_new(double real, double imaginary) {
    Complex z = {real, imaginary};
    return z;
}
static inline Complex complex_add(Complex a, Complex b) {
    return complex_new(a.re + b.re, a.im + b.im);
}
static inline Complex complex_sub(Complex a, Complex b) {
    return complex_new(a.re - b.re, a.im - b.im);
}
static inline Complex complex_mul(Complex a, Complex b) {
    return complex_new(a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re);
}
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
static inline double complex_abs(Complex z) {
    return hypot(z.re, z.im);
}
static inline void complex_print(Complex z, int digits, const char *end) {
    if (digits < 0 || digits > 100) exam_algo_fail("invalid precision");
    if (printf("%.*f %.*f%s", digits, z.re, digits, z.im, end) < 0)
        exam_algo_fail("complex output failed");
}
