int is_valid_date(Date x) {
    return x.y >= 1 && x.y <= 9999 && x.m >= 1 && x.m <= 12
        && x.d >= 1 && x.d <= days_in_month(x.y, x.m);
}
