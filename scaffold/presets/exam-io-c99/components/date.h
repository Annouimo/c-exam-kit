/** @category 大整数、枚举及其他类型 */
/* Proleptic Gregorian dates, years 1..9999, no timezone/calendar reform. */
/**
 * @public
 * @category 大整数、枚举及其他类型
 */
typedef struct { int year, month, day; } Date;
/** @public */
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
 * @public
 * @requires Date date_valid exam_algo_fail
 */
/**
 * @id date_create
 * @title 日期构造与合法性
 * @members Date date_new date_valid date_is_leap date_days_in_month date_day_of_year
 * Date 使用公历 1..9999 年。date_new 构造日期；date_valid 检查合法性；date_day_of_year 返回一年中的第几天。
 * @example
 * Date d = date_new(2024, 2, 29);
 * bool ok = date_valid(d);
 * int day = date_day_of_year(d); // 60
 */
static inline Date date_new(int year, int month, int day) {
    Date x = {year,month,day};
    if (!date_valid(x)) exam_algo_fail("invalid Gregorian date");
    return x;
}
/**
 * @public
 * @requires Date date_valid exam_algo_fail date_days_in_month
 */
static inline int date_day_of_year(Date x) {
    if (!date_valid(x)) exam_algo_fail("invalid Gregorian date");
    int count=x.day;
    for (int m=1;m<x.month;++m) count+=date_days_in_month(x.year,m);
    return count;
}
static inline int exam_date_start_of_year(int year) {
    int y=year-1;
    return 365*y+y/4-y/100+y/400;
}
/**
 * @public
 * @requires Date exam_date_start_of_year date_day_of_year
 */
static inline int date_to_serial(Date x) {
    return exam_date_start_of_year(x.year)+date_day_of_year(x)-1;
}
/**
 * @public
 * @requires Date exam_date_start_of_year exam_algo_fail date_days_in_month date_new
 */
static inline Date date_from_serial(int serial) {
    if (serial<0 || serial>=exam_date_start_of_year(10000))
        exam_algo_fail("date serial out of supported range");
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
    return date_new(left,month,remaining+1);
}
/**
 * @public
 * @requires Date date_to_serial exam_date_start_of_year exam_algo_fail date_from_serial
 */
/**
 * @id date_arithmetic
 * @title 日期计算
 * @members date_add_days date_diff date_compare date_to_serial date_from_serial date_print
 * date_add_days 返回偏移指定天数的日期；date_diff(a,b) 返回 b-a 的天数。compare 比较日期先后；serial 接口转换为连续日序号。
 * @example
 * Date a = date_new(2024, 2, 28);
 * Date b = date_add_days(a, 2);
 * int n = date_diff(a, b); // 2
 * date_print(b, "\n");
 */
static inline Date date_add_days(Date x, int delta) {
    long long serial=(long long)date_to_serial(x)+delta;
    if (serial<0 || serial>=exam_date_start_of_year(10000))
        exam_algo_fail("date shift outside supported range");
    return date_from_serial((int)serial);
}
/**
 * @public
 * @requires Date date_to_serial
 */
static inline int date_diff(Date a, Date b) {
    return date_to_serial(b)-date_to_serial(a);
}
/**
 * @public
 * @requires Date date_to_serial
 */
static inline int date_compare(Date a, Date b) {
    int x=date_to_serial(a), y=date_to_serial(b);
    return (x>y)-(x<y);
}
/**
 * @public
 * @requires Date date_valid exam_algo_fail
 */
static inline void date_print(Date x, const char *end) {
    if (!date_valid(x)) exam_algo_fail("invalid Gregorian date");
    if (printf("%04d-%02d-%02d%s",x.year,x.month,x.day,end)<0)
        exam_algo_fail("date output failed");
}
