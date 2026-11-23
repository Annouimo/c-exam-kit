/* Proleptic Gregorian dates, years 1..9999, no timezone/calendar reform. */
typedef struct { int year, month, day; } Date;
static inline bool date_is_leap(int year) {
    return year%4==0 && (year%100!=0 || year%400==0);
}
static inline int date_days_in_month(int year, int month) {
    static const int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (year<1 || year>9999 || month<1 || month>12) return 0;
    return days[month]+(month==2 && date_is_leap(year));
}
static inline bool date_valid(Date x) {
    int days=date_days_in_month(x.year,x.month);
    return days && x.day>=1 && x.day<=days;
}
static inline Date date_new(int year, int month, int day) {
    Date x = {year,month,day};
    if (!date_valid(x)) exam_algo_fail("invalid Gregorian date");
    return x;
}
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
static inline int date_to_serial(Date x) {
    return exam_date_start_of_year(x.year)+date_day_of_year(x)-1;
}
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
static inline Date date_add_days(Date x, int delta) {
    long long serial=(long long)date_to_serial(x)+delta;
    if (serial<0 || serial>=exam_date_start_of_year(10000))
        exam_algo_fail("date shift outside supported range");
    return date_from_serial((int)serial);
}
static inline int date_diff(Date a, Date b) {
    return date_to_serial(b)-date_to_serial(a);
}
static inline int date_compare(Date a, Date b) {
    int x=date_to_serial(a), y=date_to_serial(b);
    return (x>y)-(x<y);
}
static inline void date_print(Date x, const char *end) {
    if (!date_valid(x)) exam_algo_fail("invalid Gregorian date");
    if (printf("%04d-%02d-%02d%s",x.year,x.month,x.day,end)<0)
        exam_algo_fail("date output failed");
}
