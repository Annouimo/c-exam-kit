@COMMENT:feq_0@
int feq(double a, double b, double eps) {
    return fabs(a - b) <= eps;
}
