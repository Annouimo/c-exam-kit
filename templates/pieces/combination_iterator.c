@COMMENT:combination_usage@
typedef struct {
    int index[ENUM_MAX];
    int valid;
    int n, r, started, finished;
} Combination;

Combination combination(int n, int r) {
    Combination it = {0};
    if (n < 0 || n > ENUM_MAX || r < 0 || r > n) return it;
    it.valid = 1;
    it.n = n;
    it.r = r;
    for (int i = 0; i < r; ++i) it.index[i] = i;
    return it;
}

int combination_next(Combination *it) {
    if (it == NULL || !it->valid || it->finished) return 0;
    if (!it->started) {
        it->started = 1;
        return 1;
    }
    int i = it->r - 1;
    while (i >= 0 && it->index[i] == it->n - it->r + i) --i;
    if (i < 0) {
        it->finished = 1;
        return 0;
    }
    ++it->index[i];
    for (int j = i + 1; j < it->r; ++j) it->index[j] = it->index[j - 1] + 1;
    return 1;
}
