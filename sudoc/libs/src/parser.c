#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/solver.h"


void parser(char* filename, int res[][9])
{

    FILE* ptr;
    char ch;
    int row = 0;
    int col = 0;
    ptr = fopen(filename, "r");

    if (NULL == ptr) {
        printf("file can't be opened \n");
    }


    while (!feof(ptr)) {

        ch = fgetc(ptr);
        if (ch == ' ' || ch == '\n')
            continue;
        if (ch == '.')
            ch = '0';
        res[row][col] = ch - 48;
        col += 1;
        if (col == 9){
            row+=1;
            col = 0;
        }
    }
    fclose(ptr);
}

void parser_reverse(char* filename, int grid[][9])
{
    char* extension = ".result";
    char name[strlen(filename)+strlen(extension)+1];
    snprintf(name, sizeof(name), "%s%s", filename, extension);

    FILE *out=fopen(name, "w");

    for (int row = 0; row < N; row++) {
        if (row == 3 || row == 6)
            fprintf(out, "\n");
        for (int col = 0; col < N; col++){
            fprintf(out, "%c", grid[row][col] == 0 ? '.' : grid[row][col] + '0');
            if (col == 2 || col == 5)
                fprintf(out, " ");
        }
        fprintf(out, "\n");
        printf("\n");
    }


}
