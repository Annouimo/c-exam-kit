Date next_day(Date x) {
    x.d++;
    if (x.d > days_in_month(x.y, x.m)) {
        x.d = 1;
        x.m++;
        if (x.m > 12) { x.m = 1; x.y++; }
    }
    return x;
}
