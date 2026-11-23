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
static inline double round_digits(double value, int digits) {
    if (!isfinite(value) || digits < -15 || digits > 15)
        exam_algo_fail("invalid round_digits argument");
    double scale = pow(10.0, (double)digits);
    double scaled = value * scale;
    if (!isfinite(scaled)) exam_algo_fail("round_digits overflow");
    return round(scaled) / scale;
}
