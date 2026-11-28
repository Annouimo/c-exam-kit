/** @category 大整数、枚举及其他类型 */
/**
 * @public
 * @requires EXAM_ENUM_CAP
 */
typedef struct {
    int n, r, started, finished;
    int indices[EXAM_ENUM_CAP];
    int pool[EXAM_ENUM_CAP], cycles[EXAM_ENUM_CAP];
} Permutation;
/**
 * @public
 * @requires Permutation EXAM_ENUM_CAP exam_algo_fail
 */
/**
 * @id permutations
 * @title 枚举排列
 * @members Permutation permutation_new permutation_next
 * @related combinations
 * 从 0..n-1 中有序选 r 项。每次 next 返回 true 时读取 indices[0..r-1]。默认 n <= 64；r=0 枚举一次空排列。
 * @example
 * Permutation it = permutation_new(5, 3);
 * while (permutation_next(&it)) {
 *     print_ints(it.indices, it.r, " ", "\n");
 * }
 */
static inline Permutation permutation_new(int n, int r) {
    if (n < 0 || n > EXAM_ENUM_CAP || r < 0 || r > n)
        exam_algo_fail("invalid permutation dimensions");
    Permutation it = {0};
    it.n = n; it.r = r;
    for (int i = 0; i < n; ++i) it.pool[i] = i;
    for (int i = 0; i < r; ++i) it.cycles[i] = n - i;
    return it;
}
/**
 * @public
 * @requires Permutation exam_algo_fail
 */
static inline bool permutation_next(Permutation *it) {
    if (!it) exam_algo_fail("null permutation iterator");
    if (it->finished) return false;
    if (!it->started) {
        it->started = 1;
        for (int i = 0; i < it->r; ++i) it->indices[i] = it->pool[i];
        return true;
    }
    for (int i = it->r - 1; i >= 0; --i) {
        --it->cycles[i];
        if (it->cycles[i] == 0) {
            int first = it->pool[i];
            for (int j = i; j < it->n - 1; ++j) it->pool[j] = it->pool[j+1];
            it->pool[it->n-1] = first;
            it->cycles[i] = it->n - i;
        } else {
            int j = it->n - it->cycles[i];
            int tmp = it->pool[i]; it->pool[i] = it->pool[j]; it->pool[j] = tmp;
            for (int k = 0; k < it->r; ++k) it->indices[k] = it->pool[k];
            return true;
        }
    }
    it->finished = 1;
    return false;
}
