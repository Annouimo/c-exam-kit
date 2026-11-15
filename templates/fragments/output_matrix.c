@COMMENT:output_matrix_0@
for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) printf(VALUE_PRINT "%c", matrix[r][c], c+1 == cols ? '\n' : ' ');
}
