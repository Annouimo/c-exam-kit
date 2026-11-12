long long round_to_integer(double x) {
    return llround(x);
}
double round_decimal(double x, int digits) {
    double scale = pow(10.0, digits);
    return round(x * scale) / scale;
}
