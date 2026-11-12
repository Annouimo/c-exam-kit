Time next_second(Time t) {
    t.s++;
    if (t.s == 60) { t.s = 0; t.m++; }
    if (t.m == 60) { t.m = 0; t.h++; }
    if (t.h == 24) t.h = 0;
    return t;
}
