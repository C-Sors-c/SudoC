#include "../include/solver.h"
#include "../include/grid_parser.h"

bool solve_sudoku(int grid[N][N])
{
    int row, col;

    if (!find_unassigned_location(grid, &row, &col))
        return true;

    for (int num = 1; num <= 9; num++)
    {

        if (is_safe(grid, row, col, num))
        {
            grid[row][col] = num;

            if (solve_sudoku(grid))
                return true;

            grid[row][col] = UNASSIGNED;
        }
    }

    return false;
}

bool find_unassigned_location(int grid[N][N], int *row, int *col)
{
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            if (grid[x][y] == UNASSIGNED)
            {
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
bool used_in_row(int grid[N][N], int row, int num)
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
bool used_in_col(int grid[N][N], int col, int num)
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
bool used_in_box(int grid[N][N], int boxStartRow, int boxStartCol, int num)
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
bool is_safe(int grid[N][N], int row, int col, int num)
{

    /* Check if 'num' is not already placed
       in current row, current column and
       current 3x3 box */
    return grid[row][col] == UNASSIGNED &&
           !used_in_row(grid, row, num) &&
           !used_in_col(grid, col, num) &&
           !used_in_box(grid, row - row % 3, col - col % 3, num);
}

/* A utility function to print grid  */
void print_grid(int grid[N][N])
{
    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < N; col++)
            printf("%2d", grid[row][col]);
        printf("\n");
    }
}
