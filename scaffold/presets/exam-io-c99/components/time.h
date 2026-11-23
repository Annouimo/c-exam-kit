typedef struct { int hour, minute, second; } Time;
static inline bool time_valid(Time t) {
    return t.hour>=0 && t.hour<24 && t.minute>=0 && t.minute<60 &&
           t.second>=0 && t.second<60;
}
static inline Time time_new(int h, int m, int s) {
    Time t={h,m,s};
    if (!time_valid(t)) exam_algo_fail("invalid time of day");
    return t;
}
static inline int time_to_seconds(Time t) {
    if (!time_valid(t)) exam_algo_fail("invalid time of day");
    return 3600*t.hour+60*t.minute+t.second;
}
static inline Time time_from_seconds(int seconds) {
    if (seconds<0 || seconds>=86400) exam_algo_fail("invalid seconds of day");
    return time_new(seconds/3600,(seconds/60)%60,seconds%60);
}
static inline int time_diff(Time a, Time b) {
    return time_to_seconds(b)-time_to_seconds(a);
}
static inline Time time_add_seconds(Time t, int delta) {
    long long sum=(long long)time_to_seconds(t)+delta;
    int seconds=(int)((sum%86400+86400)%86400);
    return time_from_seconds(seconds);
}
static inline Time time_next_second(Time t) {
    return time_add_seconds(t,1);
}
static inline Time time_next_minute(Time t) {
    return time_add_seconds(t,60);
}
static inline void time_print(Time t,const char *end) {
    if (!time_valid(t)) exam_algo_fail("invalid time of day");
    if (printf("%02d:%02d:%02d%s",t.hour,t.minute,t.second,end)<0)
        exam_algo_fail("time output failed");
}
