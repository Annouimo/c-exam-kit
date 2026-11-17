/* SPDX-License-Identifier: MIT-0 */
#include <stdbool.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void tb_fail(const char *message) {
    fprintf(stderr, "C Toolbox: %s\n", message);
    exit(EXIT_FAILURE);
}
static size_t tb_add_size(size_t a, size_t b) {
    if (b > SIZE_MAX - a) tb_fail("size overflow");
    return a + b;
}
static void *tb_resize(void *p, size_t count, size_t width) {
    if (width && count > SIZE_MAX / width) tb_fail("size overflow");
    void *q = realloc(p, count * width);
    if (!q) tb_fail("out of memory");
    return q;
}
static size_t tb_capacity(size_t current, size_t needed) {
    size_t result = current ? current : 8;
    while (result < needed) {
        if (result > SIZE_MAX / 2) return needed;
        result *= 2;
    }
    return result;
}

