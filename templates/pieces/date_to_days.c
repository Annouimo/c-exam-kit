int date_to_days(Date x) {
    int y = x.y - 1;
    return y * 365 + y / 4 - y / 100 + y / 400 + day_of_year(x) - 1;
}
