/**
 * @public
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
 * @requires Time time_valid exam_failf
 */
static inline void exam_time_check(const char *api, const char *argument, Time t) {
    if (!time_valid(t))
        exam_failf("algorithm", api,
                   "invalid %s (hour=%d, minute=%d, second=%d; expected 0..23, 0..59, 0..59)",
                   argument, t.hour, t.minute, t.second);
}
/**
 * @requires Time
 */
static inline int exam_time_seconds(Time t) {
    return 3600 * t.hour + 60 * t.minute + t.second;
}
/**
 * @public
 * @requires Time exam_time_check
 * @id time_create
 * @title 时刻构造与合法性
 * @tags 时刻 构造 判断 转换
 * @summary 构造时刻、检查有效性以及转换为午夜起的秒数。
 * @keywords 时分秒 时间 合法性
 * @members Time time_new time_valid time_to_seconds time_from_seconds
 * Time 表示一天中的时刻。time_to_seconds 转为从午夜开始的秒数，time_from_seconds 转回 24 小时时刻。
 * @example
 * Time t = time_new(12, 30, 0);
 * bool ok = time_valid(t);
 * int s = time_to_seconds(t);
 */
static inline Time time_new(int h, int m, int s) {
    Time t={h,m,s};
    exam_time_check("time_new", "time", t);
    return t;
}
/**
 * @public
 * @requires Time exam_time_check exam_read_int_for
 * @id time_io
 * @title 读取与格式化时刻
 * @tags 时刻 输入 输出
 * @summary 读取时分秒并按指定格式输出时刻。
 * @keywords 时刻 输入 输出 读 格式 补零 时分秒 时间 格式化
 * @members time_read time_print
 * @related time_create time_arithmetic
 * time_read 按时、分、秒顺序读取三个空白分隔的整数。输入不完整或不是整数时，诊断会指出出错的字段。时刻越界时显示传入的时、分、秒。
 * time_print 按格式输出并追加 end。%H、%M、%S 默认各补至两位。字段可重排或省略；%- 去掉该字段的补零，%% 输出百分号，其余文字原样输出。格式记号非法时报告出错字节的位置。
 * 从旧版升级时，两参数调用改为 time_print(t, "%H:%M:%S", end)。
 * @example
 * Time t = time_read(); // 输入 8 5 3
 * time_print(t, "%H:%M:%S", "\n"); // 08:05:03
 * time_print(t, "%-H:%M:%S", "\n"); // 8:05:03
 */
static inline Time time_read(void) {
    Time t;
    t.hour = exam_read_int_for("time_read", "hour");
    t.minute = exam_read_int_for("time_read", "minute");
    t.second = exam_read_int_for("time_read", "second");
    exam_time_check("time_read", "time", t);
    return t;
}
/**
 * @public
 * @requires Time exam_time_check exam_time_seconds
 */
static inline int time_to_seconds(Time t) {
    exam_time_check("time_to_seconds", "time", t);
    return exam_time_seconds(t);
}
/**
 * @public
 * @requires Time exam_failf
 */
static inline Time time_from_seconds(int seconds) {
    if (seconds < 0 || seconds >= 86400)
        exam_failf("algorithm", "time_from_seconds", "seconds=%d out of range (expected 0..86399)", seconds);
    return (Time){seconds / 3600, (seconds / 60) % 60, seconds % 60};
}
/**
 * @public
 * @requires Time exam_time_check exam_time_seconds
 */
static inline int time_diff(Time a, Time b) {
    exam_time_check("time_diff", "a", a);
    exam_time_check("time_diff", "b", b);
    return exam_time_seconds(b) - exam_time_seconds(a);
}
/**
 * @requires Time exam_time_check exam_time_seconds time_from_seconds
 */
static inline Time exam_shift_time(const char *api, Time t, int delta) {
    exam_time_check(api, "base time", t);
    long long sum = (long long)exam_time_seconds(t) + delta;
    int seconds = (int)((sum % 86400 + 86400) % 86400);
    return time_from_seconds(seconds);
}
/**
 * @public
 * @requires Time exam_shift_time
 * @id time_arithmetic
 * @title 时刻计算
 * @tags 时刻 数学计算
 * @summary 计算时刻偏移和两个时刻之间的秒数。
 * @keywords 时间 下一秒 下一分钟 时间差
 * @members time_add_seconds time_diff time_next_second time_next_minute
 * 加秒、下一秒和下一分钟按 24 小时循环。time_diff(a,b) 返回 b-a 的秒数，不自动跨日。
 * @example
 * Time a = time_new(23, 59, 59);
 * Time b = time_next_second(a);
 * // b 是 00:00:00
 */
static inline Time time_add_seconds(Time t, int delta) {
    return exam_shift_time("time_add_seconds", t, delta);
}
/**
 * @public
 * @requires Time exam_shift_time
 */
static inline Time time_next_second(Time t) {
    return exam_shift_time("time_next_second", t, 1);
}
/**
 * @public
 * @requires Time exam_shift_time
 */
static inline Time time_next_minute(Time t) {
    return exam_shift_time("time_next_minute", t, 60);
}
/**
 * @public
 * @requires Time exam_time_check exam_calendar_print
 */
static inline void time_print(Time t, const char *format, const char *end) {
    exam_time_check("time_print", "time", t);
    const int values[] = {t.hour, t.minute, t.second};
    exam_calendar_print("time_print", format, end, "HMS", values);
}
