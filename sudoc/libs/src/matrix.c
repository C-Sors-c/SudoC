#include <stdio.h>
#include <stdlib.h>
#include <err.h>

struct Matrix
{
    int rows;      // number of rows
    int cols;      // number of columns
    double **data; // a pointer to an array of n_rows pointers to rows
};

struct Tuple
{
    int x; // first item
    int y; // second item
};

typedef struct Matrix Matrix;
typedef struct Tuple Tuple;

// Function: matrix_create
// -----------------------
// Allocates memory for a matrix of size rows x cols
// and returns a pointer to the matrix.
//
// Parameters:
//   rows - number of rows
//   cols - number of columns
//   datap - a pointer to an array of n_rows pointers to rows (optional)
//
// Returns:
//   a pointer to the matrix
//
Matrix *matrix_create(int rows, int cols, double **datap)
{
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
    {
        errx(EXIT_FAILURE, "matrix_create: failed to allocate memory for matrix");
        return NULL;
    }

    m->rows = rows;
    m->cols = cols;

    if (datap != NULL)
        m->data = datap;

    else
    {
        double **data = malloc(rows * sizeof(double *));
        if (data == NULL)
            err(EXIT_FAILURE, "matrix_create: malloc failed");

        for (int i = 0; i < rows; i++)
        {
            // use of calloc, it initializes the memory to zero
            data[i] = calloc(cols, sizeof(double));
        }

        m->data = data;
    }

    return m;
}

// Function: matrix_add
// --------------------
// Adds two matrices and returns the result.
//
// Parameters:
//   m1 - pointer to the first matrix
//   2 - pointer to the second matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_add(Matrix *m1, Matrix *m2)
{
    if (m1->rows != m2->rows || m1->cols != m2->cols)
    {
        errx(EXIT_FAILURE, "matrix_add: matrix dimensions do not match");
    }

    Matrix *m = matrix_create(m1->rows, m1->cols, NULL);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->data[i][j] = m1->data[i][j] + m2->data[i][j];
        }
    }
    return m;
}

// Function: matrix_substract
// --------------------
// Substracts two matrices and returns the result.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_add(Matrix *m1, Matrix *m2)
{
    if (m1->rows != m2->rows || m1->cols != m2->cols)
    {
        errx(EXIT_FAILURE, "matrix_substract: matrix dimensions do not match");
    }

    Matrix *m = matrix_create(m1->rows, m1->cols, NULL);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->data[i][j] = m1->data[i][j] - m2->data[i][j];
        }
    }
    return m;
}

// Function: matrix_print
// ----------------------
// Prints a matrix to the console.
//
// Parameters:
//   m - pointer to the matrix
//

void matrix_print(Matrix *m)
{
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            printf("%f ", m->data[i][j]);
        }
        printf("\n");
    }
}

// Function: matrix_multiply
// -------------------------
// Multiplies two matrices and returns the result.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_multiply(Matrix *m1, Matrix *m2)
{
    if (m1->cols != m2->rows)
    {
        errx(EXIT_FAILURE, "matrix_multiply: matrix dimensions do not match");
    }

    Matrix *m = matrix_create(m1->rows, m2->cols, NULL);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->data[i][j] = 0;
            for (int k = 0; k < m1->cols; k++)
            {
                m->data[i][j] += m1->data[i][k] * m2->data[k][j];
            }
        }
    }
    return m;
}

// Function: matrix_destroy
// ------------------------
// Frees the memory allocated for a matrix.
//
// Parameters:
//   m - pointer to the matrix
//

void matrix_destroy(Matrix *m)
{
    for (int i = 0; i < m->rows; i++)
    {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

// Function: matrix_dimensions
// ---------------------------
// Returns the dimensions of a matrix as a tuple.
//
// Parameters:
//   m - pointer to the matrix
//
// Returns:
//   a tuple containing the dimensions of the matrix
//

Tuple *matrix_dimensions(Matrix *m)
{
    Tuple *t;
    t->x = m->rows;
    t->y = m->cols;
    return t;
}

// Function: matrix_length
// -----------------------
// Returns the length of a matrix.
//
// Parameters:
//   m - pointer to the matrix
//
// Returns:
//   the length of the matrix
//

int matrix_length(Matrix *m)
{
    return m->rows;
}

// Function: matrix_get_element
// --------------------
// Returns the value at the specified row and column.
//
// Parameters:
//   m - pointer to the matrix
//   row - row index
//   col - column index
//
// Returns:
//   the value at the specified row and column
//

double matrix_get_element(Matrix *m, int row, int col)
{
    if (row < 0 || row >= m->rows || col < 0 || col >= m->cols)
    {
        errx(EXIT_FAILURE, "matrix_get: index out of bounds");
    }
    return m->data[row][col];
}

// Function: matrix_set_element
// --------------------
// Sets the value at the specified row and column.
//
// Parameters:
//   m - pointer to the matrix
//   row - row index
//   col - column index
//   value - value to set
//

void matrix_set_element(Matrix *m, int row, int col, double value)
{
    if (row < 0 || row >= m->rows || col < 0 || col >= m->cols)
    {
        errx(EXIT_FAILURE, "matrix_set: index out of bounds");
    }
    m->data[row][col] = value;
}

// Function: matrix_transpose
// --------------------------
// Returns the transpose of a matrix.
//
// Parameters:
//   m - pointer to the matrix
//
// Returns:
//   a pointer to the transpose of the matrix
//

Matrix *matrix_transpose(Matrix *m)
{
    Matrix *t = matrix_create(m->cols, m->rows, NULL);
    if (t == NULL)
    {
        errx(EXIT_FAILURE, "matrix_transpose: failed to allocate memory for matrix");
    }
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            t->data[j][i] = m->data[i][j];
        }
    }
    return t;
}
