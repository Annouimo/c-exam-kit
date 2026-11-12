Time next_minute(Time t) {
    t.m++;
    if (t.m == 60) { t.m = 0; t.h++; }
    if (t.h == 24) t.h = 0;
    return t;
}
