#include <stdio.h>
#include <stdlib.h>

struct Matrix
{
    int rows; // number of rows
    int cols; // number of columns

    double **data; // a pointer to an array of n_rows pointers to rows
};

typedef struct Matrix Matrix;

// Function: matrix_create
// -----------------------
// Allocates memory for a matrix of size rows x cols
// and returns a pointer to the matrix.
//
// Parameters:
//   rows - number of rows
//   cols - number of columns
//
// Returns:
//   a pointer to the matrix
//
Matrix *matrix_create(int rows, int cols)
{
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
    {
        printf(stderr, "Error: malloc failed in matrix_create");
        return NULL;
    }

    m->rows = rows;
    m->cols = cols;
    m->data = malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++)
    {
        m->data[i] = malloc(cols * sizeof(double));
    }
    return m;
}