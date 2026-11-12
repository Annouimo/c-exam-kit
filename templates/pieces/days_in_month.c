int days_in_month(int y, int m) {
    int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (m < 1 || m > 12) return 0;
    if (m == 2 && is_leap(y)) return 29;
    return days[m];
}
