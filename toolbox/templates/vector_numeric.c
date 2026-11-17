/* SPDX-License-Identifier: MIT-0 */
#include <math.h>
/* Numeric helpers for int, long long and double. Floating operands must be finite. */
static int @NAME@_compare_asc(const void *a, const void *b) {
    @TYPE@ x = *(const @TYPE@ *)a, y = *(const @TYPE@ *)b;
    return (x > y) - (x < y);
}
static int @NAME@_compare_desc(const void *a, const void *b) {
    return @NAME@_compare_asc(b, a);
}
static void @NAME@_sort_asc(@NAME@ *v) { @NAME@_sort(v, @NAME@_compare_asc); }
static void @NAME@_sort_desc(@NAME@ *v) { @NAME@_sort(v, @NAME@_compare_desc); }
static size_t @NAME@_find(const @NAME@ *v, @TYPE@ value) {
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) return i;
    return SIZE_MAX;
}
static bool @NAME@_contains(const @NAME@ *v, @TYPE@ value) {
    return @NAME@_find(v, value) != SIZE_MAX;
}
static size_t @NAME@_count(const @NAME@ *v, @TYPE@ value) {
    size_t count = 0;
    for (size_t i = 0; i < v->len; ++i) if (v->data[i] == value) ++count;
    return count;
}
static size_t @NAME@_lower_bound(const @NAME@ *v, @TYPE@ key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static size_t @NAME@_upper_bound(const @NAME@ *v, @TYPE@ key) {
    size_t left = 0, right = v->len;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (v->data[mid] <= key) left = mid + 1;
        else right = mid;
    }
    return left;
}
static void @NAME@_unique_sorted(@NAME@ *v) {
    size_t written = 0;
    for (size_t i = 0; i < v->len; ++i)
        if (!written || v->data[i] != v->data[written - 1]) v->data[written++] = v->data[i];
    v->len = written;
}
static size_t @NAME@_argmin(const @NAME@ *v) {
    if (!v->len) tb_fail("argmin of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] < v->data[best]) best = i;
    return best;
}
static size_t @NAME@_argmax(const @NAME@ *v) {
    if (!v->len) tb_fail("argmax of empty vector");
    size_t best = 0;
    for (size_t i = 1; i < v->len; ++i) if (v->data[i] > v->data[best]) best = i;
    return best;
}
static @TYPE@ @NAME@_min(const @NAME@ *v) { return v->data[@NAME@_argmin(v)]; }
static @TYPE@ @NAME@_max(const @NAME@ *v) { return v->data[@NAME@_argmax(v)]; }
static double @NAME@_mean(const @NAME@ *v) {
    if (!v->len) tb_fail("mean of empty vector");
    long double result = 0;
    for (size_t i = 0; i < v->len; ++i)
        result += ((long double)v->data[i] - result) / (long double)(i + 1);
    if (!isfinite((double)result)) tb_fail("nonfinite statistic");
    return (double)result;
}
/* Population variance, dividing by n. Original data is unchanged. */
static double @NAME@_variance(const @NAME@ *v) {
    if (!v->len) tb_fail("variance of empty vector");
    long double mean = 0, total = 0;
    for (size_t i = 0; i < v->len; ++i) {
        long double value = v->data[i], delta = value - mean;
        mean += delta / (long double)(i + 1);
        total += delta * (value - mean);
    }
    double result = (double)(total / (long double)v->len);
    if (!isfinite(result)) tb_fail("variance outside double range");
    return result;
}
static double @NAME@_stddev(const @NAME@ *v) { return sqrt(@NAME@_variance(v)); }
static double @NAME@_median(const @NAME@ *v) {
    if (!v->len) tb_fail("median of empty vector");
    @NAME@ copy = @NAME@_clone(v);
    @NAME@_sort_asc(&copy);
    size_t mid = copy.len / 2;
    long double result = copy.data[mid];
    if (!(copy.len % 2)) result = ((long double)copy.data[mid - 1] + result) / 2;
    @NAME@_free(&copy);
    if (!isfinite((double)result)) tb_fail("nonfinite median");
    return (double)result;
}
