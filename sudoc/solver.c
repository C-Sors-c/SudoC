#include "libs/include/solver.h"
#include "libs/include/parser.h"


/* Driver Program to test above functions */
int main(int argc, char** argv )
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: file\n");

    // 0 means unassigned cells
    int grid[N][N];
    parser(argv[1], grid);
    SolveSudoku(grid);
    parser_reverse(argv[1], grid);
    return 0;
}

