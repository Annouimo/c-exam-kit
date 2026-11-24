/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @category 大整数、枚举及其他类型
 */
typedef struct { int hour, minute, second; } Time;
/**
 * @public
 * @requires Time
 */
static inline bool time_valid(Time t) {
    return t.hour>=0 && t.hour<24 && t.minute>=0 && t.minute<60 &&
           t.second>=0 && t.second<60;
}
/**
 * @public
 * @requires Time time_valid exam_algo_fail
 */
/**
 * @id time_create
 * @title 时刻构造与合法性
 * @members Time time_new time_valid time_to_seconds time_from_seconds
 * Time 表示一天中的时刻。time_to_seconds 转为从午夜开始的秒数，time_from_seconds 转回 24 小时时刻。
 * @example
 * Time t = time_new(12, 30, 0);
 * bool ok = time_valid(t);
 * int s = time_to_seconds(t);
 */
static inline Time time_new(int h, int m, int s) {
    Time t={h,m,s};
    if (!time_valid(t)) exam_algo_fail("invalid time of day");
    return t;
}
/**
 * @public
 * @requires Time time_valid exam_algo_fail
 */
static inline int time_to_seconds(Time t) {
    if (!time_valid(t)) exam_algo_fail("invalid time of day");
    return 3600*t.hour+60*t.minute+t.second;
}
/**
 * @public
 * @requires Time exam_algo_fail time_new
 */
static inline Time time_from_seconds(int seconds) {
    if (seconds<0 || seconds>=86400) exam_algo_fail("invalid seconds of day");
    return time_new(seconds/3600,(seconds/60)%60,seconds%60);
}
/**
 * @public
 * @requires Time time_to_seconds
 */
static inline int time_diff(Time a, Time b) {
    return time_to_seconds(b)-time_to_seconds(a);
}
/**
 * @public
 * @requires Time time_to_seconds time_from_seconds
 */
/**
 * @id time_arithmetic
 * @title 时刻计算
 * @members time_add_seconds time_diff time_next_second time_next_minute time_print
 * 加秒、下一秒和下一分钟按 24 小时循环。time_diff(a,b) 返回 b-a 的秒数，不自动跨日。
 * @example
 * Time a = time_new(23, 59, 59);
 * Time b = time_next_second(a);
 * time_print(b, "\n");
 */
static inline Time time_add_seconds(Time t, int delta) {
    long long sum=(long long)time_to_seconds(t)+delta;
    int seconds=(int)((sum%86400+86400)%86400);
    return time_from_seconds(seconds);
}
/**
 * @public
 * @requires Time time_add_seconds
 */
static inline Time time_next_second(Time t) {
    return time_add_seconds(t,1);
}
/**
 * @public
 * @requires Time time_add_seconds
 */
static inline Time time_next_minute(Time t) {
    return time_add_seconds(t,60);
}
/**
 * @public
 * @requires Time time_valid exam_algo_fail
 */
static inline void time_print(Time t,const char *end) {
    if (!time_valid(t)) exam_algo_fail("invalid time of day");
    if (printf("%02d:%02d:%02d%s",t.hour,t.minute,t.second,end)<0)
        exam_algo_fail("time output failed");
}
