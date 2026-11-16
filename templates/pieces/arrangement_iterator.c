@COMMENT:arrangement_usage@
typedef struct {
    int index[ENUM_MAX];
    int valid;
    int n, r, started, finished;
    int state_pool[ENUM_MAX], state_cycles[ENUM_MAX];
} Arrangement;

Arrangement arrangement(int n, int r) {
    Arrangement it = {0};
    if (n < 0 || n > ENUM_MAX || r < 0 || r > n) return it;
    it.valid = 1;
    it.n = n;
    it.r = r;
    for (int i = 0; i < n; ++i) it.state_pool[i] = i;
    for (int i = 0; i < r; ++i) it.state_cycles[i] = n - i;
    return it;
}

int arrangement_next(Arrangement *it) {
    if (it == NULL || !it->valid || it->finished) return 0;
    if (!it->started) {
        it->started = 1;
        for (int i = 0; i < it->r; ++i) it->index[i] = it->state_pool[i];
        return 1;
    }
    for (int i = it->r - 1; i >= 0; --i) {
        --it->state_cycles[i];
        if (it->state_cycles[i] == 0) {
            int first = it->state_pool[i];
            for (int j = i; j < it->n - 1; ++j)
                it->state_pool[j] = it->state_pool[j + 1];
            it->state_pool[it->n - 1] = first;
            it->state_cycles[i] = it->n - i;
        } else {
            int j = it->n - it->state_cycles[i];
            int temp = it->state_pool[i];
            it->state_pool[i] = it->state_pool[j];
            it->state_pool[j] = temp;
            for (int k = 0; k < it->r; ++k) it->index[k] = it->state_pool[k];
            return 1;
        }
    }
    it->finished = 1;
    return 0;
}
