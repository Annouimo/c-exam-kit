@COMMENT:cmp_record_0@
int cmp_record(const void *a, const void *b) {
    const Record *x = (const Record *)a;
    const Record *y = (const Record *)b;
    /* 第一关键字：score 降序；第二关键字：id 升序。 */
    if (x->score != y->score) return x->score < y->score ? 1 : -1;
    return (x->id > y->id) - (x->id < y->id);
}
