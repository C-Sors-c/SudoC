#include "../include/grid_parser.h"

void load_grid(char *filename, int res[][9])
{

    FILE *ptr;
    char ch;
    int row = 0;
    int col = 0;
    ptr = fopen(filename, "r");

    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }

    while (!feof(ptr))
    {

        ch = fgetc(ptr);
        if (ch == ' ' || ch == '\n' || ch == '\r')
        {
            continue;
        }
        if (ch == '.')
        {
            res[row][col] = 0;
        }
        else
        {
            res[row][col] = ch - '0';
        }
        col++;
        if (col == 9)
        {
            col = 0;
            row++;
        }

    }
    fclose(ptr);
}

void save_grid(char *filename, int grid[][9])
{
    char *extension = ".result";
    char name[strlen(filename) + strlen(extension) + 1];
    snprintf(name, sizeof(name), "%s%s", filename, extension);

    FILE *out = fopen(name, "w");
    if (NULL == out)
        printf("file can't be opened \n");

    for (int row = 0; row < N; row++)
    {
        if (row == 3 || row == 6)
            fprintf(out, "\n");
        for (int col = 0; col < N; col++)
        {
            fprintf(out, "%c", grid[row][col] == 0 ? '.' : grid[row][col] + '0');
            if (col == 2 || col == 5)
                fprintf(out, " ");
        }
        fprintf(out, "\n");
    }
}
