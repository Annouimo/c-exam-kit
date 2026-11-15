@COMMENT:cmp_record_0@
int cmp_record(const void *a, const void *b) {
    Record x = *(const Record *)a, y = *(const Record *)b;
    if (x.score > y.score) return -1;
    if (x.score < y.score) return 1;
    if (x.id < y.id) return -1;
    if (x.id > y.id) return 1;
    return 0;
}
