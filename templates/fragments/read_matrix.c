static VALUE matrix[MATRIX_MAX][MATRIX_MAX];
int rows, cols;
scanf("%d%d", &rows, &cols);
for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) scanf(VALUE_SCAN, &matrix[r][c]);
}
