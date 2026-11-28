
/* Proleptic Gregorian dates, years 1..9999, no timezone/calendar reform. */
/**
 * @public
 */
typedef struct { int year, month, day; } Date;
/**
 * @public
 */
static inline bool date_is_leap(int year) {
    return year%4==0 && (year%100!=0 || year%400==0);
}
/**
 * @public
 * @requires date_is_leap
 */
static inline int date_days_in_month(int year, int month) {
    static const int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (year<1 || year>9999 || month<1 || month>12) return 0;
    return days[month]+(month==2 && date_is_leap(year));
}
/**
 * @public
 * @requires Date date_days_in_month
 */
static inline bool date_valid(Date x) {
    int days=date_days_in_month(x.year,x.month);
    return days && x.day>=1 && x.day<=days;
}
/**
 * @requires Date date_days_in_month exam_failf
 */
static inline void exam_date_check(const char *api, const char *argument, Date x) {
    if (x.year < 1 || x.year > 9999)
        exam_failf("algorithm", api, "%s.year=%d out of range (expected 1..9999; month=%d, day=%d)",
                   argument, x.year, x.month, x.day);
    if (x.month < 1 || x.month > 12)
        exam_failf("algorithm", api, "%s.month=%d out of range (expected 1..12; year=%d, day=%d)",
                   argument, x.month, x.year, x.day);
    int max_day = date_days_in_month(x.year, x.month);
    if (x.day < 1 || x.day > max_day)
        exam_failf("algorithm", api, "%s.day=%d out of range for %04d-%02d (expected 1..%d)",
                   argument, x.day, x.year, x.month, max_day);
}
/**
 * @public
 * @requires Date exam_date_check
 * @id date_create
 * @title 日期构造与合法性
 * @tags 日期 构造 判断
 * @summary 构造日期、检查有效性和获取公历日期信息。
 * @keywords 年月日 闰年 合法性
 * @members Date date_new date_valid date_is_leap date_days_in_month date_day_of_year
 * Date 使用公历 1..9999 年。date_new 构造日期；date_valid 检查合法性；date_day_of_year 返回一年中的第几天。
 * @example
 * Date d = date_new(2024, 2, 29);
 * bool ok = date_valid(d);
 * int day = date_day_of_year(d); // 60
 */
static inline Date date_new(int year, int month, int day) {
    Date x = {year,month,day};
    exam_date_check("date_new", "date", x);
    return x;
}
/**
 * @requires Date date_days_in_month
 */
static inline int exam_date_day_of_year(Date x) {
    int count=x.day;
    for (int m=1;m<x.month;++m) count+=date_days_in_month(x.year,m);
    return count;
}
/**
 * @public
 * @requires Date exam_date_check exam_read_int_for
 * @id date_io
 * @title 读取与格式化日期
 * @tags 日期 输入 输出
 * @summary 读取年月日并按指定格式输出日期。
 * @keywords 日期 输入 输出 读 格式 补零 年月日 格式化
 * @members date_read date_print
 * @related date_create date_arithmetic
 * date_read 按年、月、日顺序读取三个空白分隔的整数。日期非法、输入不完整或不是整数时，诊断会指出出错的字段。
 * date_print 按格式输出并追加 end。%Y 补至四位，%m、%d 补至两位。字段可重排或省略；%- 去掉该字段的补零，%% 输出百分号，其余文字原样输出。格式记号非法时报告出错字节的位置。
 * 从旧版升级时，两参数调用改为 date_print(d, "%Y-%m-%d", end)。
 * @example
 * Date d = date_read(); // 输入 2024 2 9
 * date_print(d, "%Y-%m-%d", "\n"); // 2024-02-09
 * date_print(d, "%Y年%-m月%-d日", "\n"); // 2024年2月9日
 */
static inline Date date_read(void) {
    Date x;
    x.year = exam_read_int_for("date_read", "year");
    x.month = exam_read_int_for("date_read", "month");
    x.day = exam_read_int_for("date_read", "day");
    exam_date_check("date_read", "date", x);
    return x;
}
/**
 * @public
 * @requires Date exam_date_check exam_date_day_of_year
 */
static inline int date_day_of_year(Date x) {
    exam_date_check("date_day_of_year", "date", x);
    return exam_date_day_of_year(x);
}
static inline int exam_date_start_of_year(int year) {
    int y=year-1;
    return 365*y+y/4-y/100+y/400;
}
/**
 * @requires Date exam_date_start_of_year exam_date_day_of_year
 */
static inline int exam_date_serial(Date x) {
    return exam_date_start_of_year(x.year) + exam_date_day_of_year(x) - 1;
}
/**
 * @public
 * @requires Date exam_date_check exam_date_serial
 */
static inline int date_to_serial(Date x) {
    exam_date_check("date_to_serial", "date", x);
    return exam_date_serial(x);
}
/**
 * @public
 * @requires Date exam_date_start_of_year exam_failf date_days_in_month
 */
static inline Date date_from_serial(int serial) {
    if (serial<0 || serial>=exam_date_start_of_year(10000))
        exam_failf("algorithm", "date_from_serial", "serial=%d out of range (expected 0..%d)",
                   serial, exam_date_start_of_year(10000) - 1);
    int left=1, right=10000;
    while (left+1<right) {
        int mid=left+(right-left)/2;
        if (exam_date_start_of_year(mid)<=serial) left=mid;
        else right=mid;
    }
    int remaining=serial-exam_date_start_of_year(left);
    int month=1;
    while (remaining>=date_days_in_month(left,month)) {
        remaining-=date_days_in_month(left,month);++month;
    }
    return (Date){left, month, remaining + 1};
}
/**
 * @public
 * @requires Date date_valid exam_date_serial exam_date_start_of_year exam_failf date_from_serial
 * @id date_arithmetic
 * @title 日期计算
 * @tags 日期 数学计算 转换
 * @summary 计算日期偏移、日期差、先后顺序及日序号。
 * @keywords 相差天数 日期加减 序号
 * @members date_add_days date_diff date_compare date_to_serial date_from_serial
 * date_add_days 返回偏移指定天数的日期；date_diff(a,b) 返回 b-a 的天数。compare 比较日期先后；serial 接口转换为连续日序号。
 * @example
 * Date a = date_new(2024, 2, 28);
 * Date b = date_add_days(a, 2);
 * int n = date_diff(a, b); // 2
 * // b 是 2024-03-01
 */
static inline Date date_add_days(Date x, int delta) {
    if (!date_valid(x))
        exam_failf("algorithm", "date_add_days",
                   "invalid base date (year=%d, month=%d, day=%d), delta=%d",
                   x.year, x.month, x.day, delta);
    long long serial = (long long)exam_date_serial(x) + delta;
    if (serial < 0 || serial >= exam_date_start_of_year(10000))
        exam_failf("algorithm", "date_add_days",
                   "result outside 0001-01-01..9999-12-31 (base=%04d-%02d-%02d, delta=%d)",
                   x.year, x.month, x.day, delta);
    return date_from_serial((int)serial);
}
/**
 * @public
 * @requires Date exam_date_check exam_date_serial
 */
static inline int date_diff(Date a, Date b) {
    exam_date_check("date_diff", "a", a);
    exam_date_check("date_diff", "b", b);
    return exam_date_serial(b) - exam_date_serial(a);
}
/**
 * @public
 * @requires Date exam_date_check exam_date_serial
 */
static inline int date_compare(Date a, Date b) {
    exam_date_check("date_compare", "a", a);
    exam_date_check("date_compare", "b", b);
    int x = exam_date_serial(a), y = exam_date_serial(b);
    return (x>y)-(x<y);
}
/**
 * @public
 * @requires Date exam_date_check exam_calendar_print
 */
static inline void date_print(Date x, const char *format, const char *end) {
    exam_date_check("date_print", "date", x);
    const int values[] = {x.year, x.month, x.day};
    exam_calendar_print("date_print", format, end, "Ymd", values);
}
