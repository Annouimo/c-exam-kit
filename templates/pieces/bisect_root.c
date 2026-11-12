double bisect_root(double left, double right, double eps) {
    double fl = root_f(left), fr = root_f(right);
    if (fl == 0) return left;
    if (fr == 0) return right;
    while (right - left > eps) {
        double mid = left + (right - left) / 2;
        if (mid == left || mid == right) break;  /* 避免浮点精度停滞 */
        double fm = root_f(mid);
        if (fm == 0) return mid;
        if ((fl < 0 && fm < 0) || (fl > 0 && fm > 0)) {
            left = mid;
            fl = fm;
        } else right = mid;
    }
    return left + (right - left) / 2;
}
