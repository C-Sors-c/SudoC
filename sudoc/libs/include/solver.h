#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <err.h>
#define N 9
#define UNASSIGNED 0

bool SolveSudoku(int grid[N][N]);
bool FindUnassignedLocation(int grid[N][N], int *row, int *col);
bool isSafe(int grid[N][N], int row, int col, int num);
void printGrid(int grid[N][N]);
