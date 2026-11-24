/** @category 输入和输出 */

/**
 * @public
 * @requires try_read_int exam_io_fail
 * @category 输入和输出
 */
/**
 * @id read_number
 * @title 读一个数
 * @keywords 数字 整数 浮点 scanf
 * @variant int read_int
 * @variant ll read_ll
 * @variant double read_double
 * 从标准输入读取指定类型的一个数。提前遇到 EOF 或输入无效时结束程序。
 * @example
 * {{type}} x = {{fn1}}();
 */
static inline int read_int(void) {
    int value;
    if (!try_read_int(&value)) exam_io_fail("unexpected EOF while reading int");
    return value;
}

/**
 * @public
 * @requires try_read_ll exam_io_fail
 */
static inline long long read_ll(void) {
    long long value;
    if (!try_read_ll(&value)) exam_io_fail("unexpected EOF while reading long long");
    return value;
}

/**
 * @public
 * @requires try_read_double exam_io_fail
 */
static inline double read_double(void) {
    double value;
    if (!try_read_double(&value)) exam_io_fail("unexpected EOF while reading double");
    return value;
}
