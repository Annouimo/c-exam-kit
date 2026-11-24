/** @category 数组 */
/** @public */
/**
 * @id record_type
 * @title 定义结构体与比较函数
 * @members Record
 * @related sort_records find_records_by
 * 先包含 exam.h，再定义命名结构体 struct Record（不是匿名 typedef）。比较函数类型为 int compare(const Record *, const Record *)；负数表示前者排前，0 表示相等，正数表示后者排前。
 * @example
 * #include "exam.h"
 * struct Record { int id; };
 * static int cmp(const Record *a, const Record *b) {
 *     return (a->id > b->id) - (a->id < b->id);
 * }
 * int main(void) {
 *     Record a[] = {{2}, {1}};
 *     sort_records(a, 2, cmp);
 *     return 0;
 * }
 */
typedef struct Record Record;
