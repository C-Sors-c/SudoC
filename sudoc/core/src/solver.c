#include "../../libs/include/solver.h"
#include "../../libs/include/parser.h"


/* Driver Program to test above functions */
int main(int argc, char** argv )
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: file\n");

    // 0 means unassigned cells
    int grid[N][N];
    int grid2[N][N] = {
        { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
        { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
        { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
        { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
        { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
        { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
        { 0, 0, 5, 2, 0, 6, 3, 0, 0 } };
    parser(argv[1], grid);
    SolveSudoku(grid);
    parser_reverse(argv[1], grid);
    return 0;
}

