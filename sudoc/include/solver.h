#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <err.h>
#define N 9
#define UNASSIGNED 0

bool solve_sudoku(int grid[N][N]);
bool find_unassigned_location(int grid[N][N], int *row, int *col);
bool is_safe(int grid[N][N], int row, int col, int num);
void print_grid(int grid[N][N]);
