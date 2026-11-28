/**
 * @public
 * @id record_type
 * @title 定义结构体与比较函数
 * @tags 结构体 构造
 * @summary 定义具名 Record 结构体及比较规则。
 * @keywords 自定义类型 Record 比较函数 struct
 * @members Record
 * @related sort_records find_records_by
 * 保留题目原有的脚手架头文件引用，在其下方定义命名结构体 struct Record（不是匿名 typedef）。比较函数类型为 int compare(const Record *, const Record *)；负数表示前者排前，0 表示相等，正数表示后者排前。
 * @example
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
