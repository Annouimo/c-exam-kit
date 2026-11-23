#ifndef EXAM_ENUM_CAP
#define EXAM_ENUM_CAP 64
#endif
typedef struct {
    int n, r, started, finished;
    int indices[EXAM_ENUM_CAP];
} Combination;
static inline Combination combination_new(int n, int r) {
    if (n < 0 || n > EXAM_ENUM_CAP || r < 0 || r > n)
        exam_algo_fail("invalid combination dimensions");
    Combination it = {0};
    it.n = n; it.r = r;
    for (int i = 0; i < r; ++i) it.indices[i] = i;
    return it;
}
static inline bool combination_next(Combination *it) {
    if (!it) exam_algo_fail("null combination iterator");
    if (it->finished) return false;
    if (!it->started) { it->started = 1; return true; }
    int i = it->r - 1;
    while (i >= 0 && it->indices[i] == it->n - it->r + i) --i;
    if (i < 0) { it->finished = 1; return false; }
    ++it->indices[i];
    for (int j = i + 1; j < it->r; ++j)
        it->indices[j] = it->indices[j-1] + 1;
    return true;
}
