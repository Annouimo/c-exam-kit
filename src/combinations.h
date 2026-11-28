
#ifndef EXAM_ENUM_CAP
#define EXAM_ENUM_CAP 64
#endif
/**
 * @public
 * @requires EXAM_ENUM_CAP
 */
typedef struct {
    int n, r, started, finished;
    int indices[EXAM_ENUM_CAP];
} Combination;
/**
 * @public
 * @requires Combination EXAM_ENUM_CAP exam_algo_fail
 * @id combinations
 * @title 枚举组合
 * @tags 枚举 数组
 * @summary 枚举从 n 项中无序选择 r 项的组合。
 * @keywords 组合 选取 不重复
 * @members Combination combination_new combination_next
 * @related permutations
 * 从 0..n-1 中无序选 r 项。初始化迭代器，每次 next 返回 true 时读取 indices[0..r-1]。默认 n <= 64；r=0 枚举一次空组合。
 * @example
 * Combination it = combination_new(5, 3);
 * while (combination_next(&it)) {
 *     print_ints(it.indices, it.r, " ", "\n");
 * }
 */
static inline Combination combination_new(int n, int r) {
    if (n < 0 || n > EXAM_ENUM_CAP || r < 0 || r > n)
        exam_algo_fail("invalid combination dimensions");
    Combination it = {0};
    it.n = n; it.r = r;
    for (int i = 0; i < r; ++i) it.indices[i] = i;
    return it;
}
/**
 * @public
 * @requires Combination exam_algo_fail
 */
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
