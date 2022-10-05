#include "../include/solver.h"
#include "../include/grid_parser.h"

bool SolveSudoku(int grid[N][N])
{
    int row, col;

    if (!FindUnassignedLocation(grid, &row, &col))
        return true;

    for (int num = 1; num <= 9; num++)
    {

        if (isSafe(grid, row, col, num))
        {
            grid[row][col] = num;

            if (SolveSudoku(grid))
                return true;

            grid[row][col] = UNASSIGNED;
        }
    }

    return false;

}


bool FindUnassignedLocation(int grid[N][N], int *row, int *col)
{
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            if (grid[x][y] == UNASSIGNED){
                *row = x;
                *col = y;
                return true;
            }
    return false;
}

/* Returns a boolean which indicates
   whether an assigned entry
   in the specified row matches the
   given number. */
bool UsedInRow(
        int grid[N][N], int row, int num)
{
    for (int col = 0; col < N; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}

/* Returns a boolean which indicates
   whether an assigned entry
   in the specified column matches
   the given number. */
bool UsedInCol(
        int grid[N][N], int col, int num)
{
    for (int row = 0; row < N; row++)
        if (grid[row][col] == num)
            return true;
    return false;
}

/* Returns a boolean which indicates
   whether an assigned entry
   within the specified 3x3 box
   matches the given number. */
bool UsedInBox(
        int grid[N][N], int boxStartRow,
        int boxStartCol, int num)
{
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (
                    grid[row + boxStartRow]
                    [col + boxStartCol] ==
                    num)
                return true;
    return false;
}

/* Returns a boolean which indicates
   whether it will be legal to assign
   num to the given row, col location. */
bool isSafe(
        int grid[N][N], int row,
        int col, int num)
{

    /* Check if 'num' is not already placed
       in current row, current column and
       current 3x3 box */
    return grid[row][col] == UNASSIGNED && !UsedInRow(grid, row, num)
        && !UsedInCol(grid, col, num)
        && !UsedInBox(grid, row - row % 3, col - col % 3, num);
}

/* A utility function to print grid  */
void printGrid(int grid[N][N])
{
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++)
            printf("%2d", grid[row][col]);
        printf("\n");
    }
}

