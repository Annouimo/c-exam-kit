/* SPDX-License-Identifier: MIT-0 */
static int compare_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y); /* smallest value first; reverse for a max-heap */
}
int main(void) {
    IntHeap pending = IntHeap_new(compare_int);
    IntHeap_push(&pending, 30);
    IntHeap_push(&pending, 10);
    IntHeap_push(&pending, 20);
    int value;
    while (IntHeap_pop(&pending, &value)) printf("%d\n", value);
    IntHeap_free(&pending);
    return 0;
}
