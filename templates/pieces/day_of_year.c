int day_of_year(Date x) {
    int result = x.d;
    for (int m = 1; m < x.m; m++) result += days_in_month(x.y, m);
    return result;
}
