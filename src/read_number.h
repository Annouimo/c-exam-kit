/**
 * @requires exam_try_read_int exam_failf
 */
static inline int exam_read_int_for(const char *api, const char *field) {
    int value;
    if (!exam_try_read_int(api, field, &value))
        exam_failf("io", api, "unexpected EOF while reading %s", field ? field : "int");
    return value;
}

/**
 * @public
 * @requires exam_read_int_for
 * @id read_number
 * @title 读一个数
 * @tags 输入 数值
 * @summary 从标准输入读取一个指定类型的数。
 * @error 提前 EOF、格式无效或数值越界时结束程序。
 * @keywords 数字 整数 浮点 scanf 读数
 * @variant int read_int
 * @variant ll read_ll
 * @variant double read_double
 * 从标准输入读取指定类型的一个数。正常读取之外，提前遇到 EOF、格式无效和数值超出范围分别报错。格式或范围错误会显示一段输入。
 * @example
 * {{type}} x = {{fn1}}();
 */
static inline int read_int(void) {
    return exam_read_int_for("read_int", NULL);
}

/**
 * @public
 * @requires exam_try_read_ll exam_failf
 */
static inline long long read_ll(void) {
    long long value;
    if (!exam_try_read_ll("read_ll", NULL, &value))
        exam_failf("io", "read_ll", "unexpected EOF while reading long long");
    return value;
}

/**
 * @public
 * @requires exam_try_read_double exam_failf
 */
static inline double read_double(void) {
    double value;
    if (!exam_try_read_double("read_double", NULL, &value))
        exam_failf("io", "read_double", "unexpected EOF while reading double");
    return value;
}
