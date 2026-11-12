/* TODO: matrix、rows、cols为计算后的矩阵与维数 */
for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) printf(VALUE_PRINT "%c", matrix[r][c], c+1 == cols ? '\n' : ' ');
}
