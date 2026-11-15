@COMMENT:dfs_grid_0@
int dfs_grid(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return 0;
    if (grid[r][c] != 1 || visited[r][c]) return 0;
    visited[r][c] = 1;
    int count = 1;
    for (int k = 0; k < 4; k++) count += dfs_grid(r + dr[k], c + dc[k]);
    return count;
}
