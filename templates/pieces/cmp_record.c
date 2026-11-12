/* 先 score 降序，再 id 升序；改字段／比较方向即可换题 */
int cmp_record(const void *a, const void *b) {
    Record x = *(const Record *)a, y = *(const Record *)b;
    if (x.score > y.score) return -1;
    if (x.score < y.score) return 1;
    if (x.id < y.id) return -1;
    if (x.id > y.id) return 1;
    return 0;
}
