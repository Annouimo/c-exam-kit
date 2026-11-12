QuadraticResult solve_quadratic(double a, double b, double c, double eps) {
    QuadraticResult r = {0, 0, 0, 0, 0};
    if (fabs(a) <= eps) { r.kind = -1; return r; }
    double delta = b*b - 4*a*c;
    if (fabs(delta) <= eps) {
        r.kind = 1;
        r.x1 = r.x2 = -b / (2*a);
    } else if (delta > 0) {
        r.kind = 2;
        /* 稳定的等价公式，减少 -b 与 sqrt(delta) 相减时的消去误差 */
        double q = -0.5 * (b + (b >= 0 ? sqrt(delta) : -sqrt(delta)));
        r.x1 = q / a;
        r.x2 = c / q;
        if (r.x1 > r.x2) {
            double t = r.x1; r.x1 = r.x2; r.x2 = t;
        }
    } else {
        r.kind = 0;
        r.re = -b / (2*a);
        r.im = sqrt(-delta) / (2*fabs(a));
    }
    return r;
}
