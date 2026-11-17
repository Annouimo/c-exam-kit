/* SPDX-License-Identifier: MIT-0 */
static int compare_student(const void *a, const void *b) {
    const Student *x = a, *y = b;
    return (x->score < y->score) - (x->score > y->score);
}
int main(void) {
    StudentVector students = StudentVector_new();
    StudentVector_push(&students, (Student){1, 80});
    StudentVector_push(&students, (Student){2, 95});
    StudentVector_sort(&students, compare_student);
    for (size_t i = 0; i < students.len; ++i)
        printf("%d %d\n", students.data[i].id, students.data[i].score);
    StudentVector_free(&students);
    return 0;
}
