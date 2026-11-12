/* 需要 math.h */
int feq(double a, double b, double eps) {
    return fabs(a - b) <= eps;
}
