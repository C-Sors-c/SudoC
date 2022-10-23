#include "include/solver.h"
#include "include/grid_parser.h"

/* Driver Program to test above functions */
int main(int argc, char **argv)
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: file\n");

    // 0 means unassigned cells
    int grid[N][N];
    load_grid(argv[1], grid);
    SolveSudoku(grid);
    save_grid(argv[1], grid);
    return 0;
}
