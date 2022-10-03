#include "../include/matrix.h"

/*
 How to create a new matrix?

1. Use a unidimensional array to store the data you want to store in the
   matrix
2. Use the function matrix_new to create a new matrix
   IMPORTANT: set rows and columns correctly considering the data you want to
   store
3. Example:
   float data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
   Matrix *m = matrix_init(3, 3, data);
   matrix_print(m);
   matrix_destroy(m);
   !!!!DONT FORGET TO FREE THE MEMORY!!!
*/

struct Tuple
{
    int x; // first item
    int y; // second item
};

struct Matrix
{
    int rows;     // number of rows
    int cols;     // number of columns
    float **data; // a pointer to an array of pointers to rows
};

struct Matrix3d
{
    int rows;  // number of rows
    int cols;  // number of columns
    int depth; // depth
    float ***data;
};

struct Matrix4d
{
    int rows;   // number of rows
    int cols;   // number of columns
    int depth;  // depth
    int depth2; // depth
    float ****data;
};

typedef struct Tuple Tuple;
typedef struct Matrix Matrix;
typedef struct Matrix3d Matrix3d;
typedef struct Matrix4d Matrix4d;

// Function: matrix_init
// -----------------------
// Allocates memory for a matrix of size rows x cols
// and returns a pointer to the matrix.
//
// Parameters:
//   rows - number of rows
//   cols - number of columns
//   datap - a pointer to an array of n_rows (optional)
//
// Returns:
//   a pointer to the matrix
//
Matrix *matrix_init(int rows, int cols, float *datap)
{
    Matrix *m = malloc(sizeof(Matrix));

    if (m == NULL)
    {
        errx(EXIT_FAILURE, "matrix_init: failed to allocate memory for matrix\n");
        return NULL;
    }

    m->rows = rows;
    m->cols = cols;
    float **data = malloc(rows * sizeof(float *));

    if (data == NULL)
        err(EXIT_FAILURE, "matrix_init: malloc failed\n");

    if (datap != NULL)
    {
        for (int i = 0; i < rows; i++)
        {
            data[i] = malloc(cols * sizeof(float *));

            for (int j = 0; j < cols; j++)
            {
                data[i][j] = datap[i * cols + j];
            }
        }
    }
    else
    {
        for (int i = 0; i < rows; i++)
        {
            // use of calloc, it initializes the memory to zero
            data[i] = calloc(cols, sizeof(float));
        }
    }
    m->data = data;
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
        errx(EXIT_FAILURE, "matrix_add: matrix dimensions do not match\n");
    }

    Matrix *m = matrix_init(m1->rows, m1->cols, NULL);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->data[i][j] = m1->data[i][j] + m2->data[i][j];
        }
    }
    return m;
}

// Function: matrix_subtract
// --------------------
// Subtracts two matrices and returns the result.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_subtract(Matrix *m1, Matrix *m2)
{
    if (m1->rows != m2->rows || m1->cols != m2->cols)
    {
        errx(EXIT_FAILURE, "matrix_subtract: matrix dimensions do not match\n");
    }

    Matrix *m = matrix_init(m1->rows, m1->cols, NULL);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->data[i][j] = m1->data[i][j] - m2->data[i][j];
        }
    }
    return m;
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
        errx(EXIT_FAILURE, "matrix_multiply: matrix dimensions do not match\n");
    }

    Matrix *m = matrix_init(m1->rows, m2->cols, NULL);
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

// Function: matrix_scalar_multiply
// --------------------------------
// Multiplies a matrix by a scalar.
//
// Parameters:
//   m - pointer to the matrix
//   scalar - the scalar
//

void matrix_scalar_multiply(Matrix *m, float scalar)
{
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->data[i][j] *= scalar;
        }
    }
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
    Tuple *t = malloc(sizeof(Tuple));
    t->x = m->rows;
    t->y = m->cols;
    return t;
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

float matrix_get_element(Matrix *m, int row, int col)
{
    if (row < 0 || row >= m->rows || col < 0 || col >= m->cols)
    {
        errx(EXIT_FAILURE, "matrix_get: index out of bounds\n");
    }
    return m->data[row][col];
}

// Function: matrix_set
// --------------------
// Sets the value at the specified row and column.
//
// Parameters:
//   m - pointer to the matrix
//   row - row index
//   col - column index
//   value - value to set
//

void matrix_set(Matrix *m, int row, int col, float value)
{
    if (row < 0 || row >= m->rows || col < 0 || col >= m->cols)
    {
        errx(EXIT_FAILURE, "matrix_set: index out of bounds\n");
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
    Matrix *t = matrix_init(m->cols, m->rows, NULL);
    if (t == NULL)
    {
        errx(EXIT_FAILURE,
             "matrix_transpose: failed to allocate memory for matrix\n");
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

// Function: matrix_elementwise_equal
// ----------------------
// Checks if two matrices are elementwise equal.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//

bool matrix_element_wise_equal(Matrix *m1, Matrix *m2)
{
    if (m1->rows != m2->rows || m1->cols != m2->cols)
    {
        errx(EXIT_FAILURE, "matrix_element_wise_equal: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m1->cols; j++)
        {
            if (m1->data[i][j] != m2->data[i][j])
            {
                return false;
            }
        }
    }

    return true;
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