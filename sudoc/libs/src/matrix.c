#include "../include/matrix.h"

void malloc_error() { errx(EXIT_FAILURE, "Error allocating memory"); }

#pragma region matrix

/*
How to create a new matrix?

1. Use a unidimensional array to store the data you want to store in the
   matrix
2. Use the function matrix_new to create a new matrix
   IMPORTANT: set dim1 and columns correctly considering the data you want to
   store
3. Example:
   float data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
   Matrix *m = matrix_init(3, 3, data);
   matrix_print(m);
   matrix_destroy(m);
   !!!!DONT FORGET TO FREE THE MEMORY!!!
*/

struct Matrix
{
    int dim1; // number of rows
    int dim2; // number of columns
    float **data;
};

typedef struct Matrix Matrix;

// Function: matrix_init
// -----------------------
// Allocates memory for a matrix of size dim1 x dim2
// and returns a pointer to the matrix.
//
// Parameters:
//   dim1 - number of rows
//   dim2 - number of columns
//   datap - a pointer to an array of n_rows (optional)
//
// Returns:
//   a pointer to the matrix
//
Matrix *matrix_init(int dim1, int dim2, float *datap)
{
    Matrix *m = malloc(sizeof(Matrix));
    if (m == NULL)
        malloc_error();

    m->dim1 = dim1;
    m->dim2 = dim2;
    m->data = malloc(dim1 * sizeof(float *));

    if (m->data == NULL)
        malloc_error();

    if (datap != NULL)
    {
        for (int i = 0; i < dim1; i++)
        {
            m->data[i] = malloc(dim2 * sizeof(float *));
            if (m->data[i] == NULL)
                malloc_error();

            for (int j = 0; j < dim2; j++)
            {
                m->data[i][j] = datap[i * dim2 + j];
            }
        }
    }
    else
    {
        for (int i = 0; i < dim1; i++)
        {
            // use of calloc, it initializes the memory to zero
            m->data[i] = calloc(dim2, sizeof(float));
        }
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
    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2)
    {
        errx(EXIT_FAILURE, "matrix_add: matrix dimensions do not match\n");
    }

    Matrix *m = matrix_init(m1->dim1, m1->dim2, NULL);
    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
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
    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2)
    {
        errx(EXIT_FAILURE, "matrix_subtract: matrix dimensions do not match\n");
    }

    Matrix *m = matrix_init(m1->dim1, m1->dim2, NULL);
    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
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
    if (m1->dim2 != m2->dim1)
    {
        errx(EXIT_FAILURE, "matrix_multiply: matrix dimensions do not match\n");
    }

    Matrix *m = matrix_init(m1->dim1, m2->dim2, NULL);
    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
        {
            m->data[i][j] = 0;
            for (int k = 0; k < m1->dim2; k++)
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

void matrix_scalar_multiply(Matrix *m, float s)
{
    for (int i = 0; i < m->dim1; i++)
        for (int j = 0; j < m->dim2; j++)
            m->data[i][j] *= s;
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
    for (int i = 0; i < m->dim1; i++)
    {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

// Function: matrix_get
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

float matrix_get(Matrix *m, int row, int col)
{
    if (row < 0 || row >= m->dim1 || col < 0 || col >= m->dim2)
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
    if (row < 0 || row >= m->dim1 || col < 0 || col >= m->dim2)
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
    Matrix *t = matrix_init(m->dim2, m->dim1, NULL);
    if (t == NULL)
    {
        errx(EXIT_FAILURE,
             "matrix_transpose: failed to allocate memory for matrix\n");
    }
    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
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
    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2)
    {
        errx(EXIT_FAILURE, "matrix_element_wise_equal: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m1->dim1; i++)
    {
        for (int j = 0; j < m1->dim2; j++)
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
    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
        {
            printf("%f ", m->data[i][j]);
        }
        printf("\n");
    }
}

#pragma endregion matrix

#pragma region matrix4

struct Matrix4
{
    int dim1;
    int dim2;
    int dim3;
    int dim4;
    float ****data;
};
typedef struct Matrix4 Matrix4;

// Function: matrix4_init
// ----------------------
// Initializes a 4-dimensional matrix.
//
// Parameters:
//   dim1 - size of dimension 1
//   dim2 - size of dimension 2
//   dim3 - size of dimension 3
//   dim4 - size of dimension 4
//   data - pointer to the data
//
// Returns:
//   pointer to the matrix
//

Matrix4 *matrix4_init(int dim1, int dim2, int dim3, int dim4, float *datap)
{
    Matrix4 *m = malloc(sizeof(Matrix4));
    if (m == NULL)
    {
        malloc_error();
    }

    m->dim1 = dim1;
    m->dim2 = dim2;
    m->dim3 = dim3;
    m->dim4 = dim4;

    m->data = malloc(dim1 * sizeof(float ****));
    if (m->data == NULL)
        malloc_error();

    for (int i = 0; i < dim1; i++)
    {
        m->data[i] = malloc(dim2 * sizeof(float ***));
        if (m->data[i] == NULL)
            malloc_error();

        for (int j = 0; j < dim2; j++)
        {
            m->data[i][j] = malloc(dim3 * sizeof(float **));
            if (m->data[i][j] == NULL)
                malloc_error();

            for (int k = 0; k < dim3; k++)
            {

                if (datap != NULL)
                {
                    m->data[i][j][k] = malloc(dim4 * sizeof(float *));

                    if (m->data[i] == NULL)
                        malloc_error();

                    for (int l = 0; l < dim4; l++)
                    {
                        // initialize data from datap
                        m->data[i][j][k][l] = datap[i * dim2 * dim3 * dim4 + j * dim3 * dim4 + k * dim4 + l];
                    }
                }
                else
                {
                    // use of calloc, it initializes the memory to zero
                    m->data[i][j][k] = calloc(dim4, sizeof(float));
                }
            }
        }
    }
    return m;
}

// Function: matrix4_add
// ---------------------------------
// Adds two matrices element-wise.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   pointer to the resulting matrix
//
Matrix4 *matrix4_add(Matrix4 *m1, Matrix4 *m2)
{
    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || m1->dim3 != m2->dim3 || m1->dim4 != m2->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_add: matrix dimensions do not match\n");
    }

    Matrix4 *m = matrix4_init(m1->dim1, m1->dim2, m1->dim3, m1->dim4, NULL);

    for (int i = 0; i < m1->dim1; i++)
        for (int j = 0; j < m1->dim2; j++)
            for (int k = 0; k < m1->dim3; k++)
                for (int l = 0; l < m1->dim4; l++)
                    m->data[i][j][k][l] = m1->data[i][j][k][l] + m2->data[i][j][k][l];

    return m;
}

// Function: matrix4_subtract
// ---------------------------------
// Subtracts two matrices element-wise.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   pointer to the resulting matrix
//
Matrix4 *matrix4_subtract(Matrix4 *m1, Matrix4 *m2)
{
    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || m1->dim3 != m2->dim3 || m1->dim4 != m2->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_subtract: matrix dimensions do not match\n");
    }

    Matrix4 *m = matrix4_init(m1->dim1, m1->dim2, m1->dim3, m1->dim4, NULL);

    for (int i = 0; i < m1->dim1; i++)
        for (int j = 0; j < m1->dim2; j++)
            for (int k = 0; k < m1->dim3; k++)
                for (int l = 0; l < m1->dim4; l++)
                    m->data[i][j][k][l] = m1->data[i][j][k][l] - m2->data[i][j][k][l];

    return m;
}

// Function: matrix4_multiply
// --------------------------
// Multiplies two matrices.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   pointer to the resulting matrix
//
Matrix4 *matrix4_multiply(Matrix4 *m1, Matrix4 *m2)
{
    if (m1->dim4 != m2->dim3)
    {
        errx(EXIT_FAILURE, "matrix4_multiply: matrix dimensions do not match\n");
    }

    Matrix4 *m = matrix4_init(m1->dim1, m1->dim2, m1->dim3, m2->dim4, NULL);

    for (int i = 0; i < m1->dim1; i++)
        for (int j = 0; j < m1->dim2; j++)
            for (int k = 0; k < m1->dim3; k++)
                for (int l = 0; l < m2->dim4; l++)
                    for (int n = 0; n < m1->dim4; n++)
                        m->data[i][j][k][l] += m1->data[i][j][k][n] * m2->data[i][j][n][l];

    return m;
}

// Function: matrix4_transpose
// ---------------------------
// Transposes a 4D matrix.
// CAREFUL ! THIS IS NOT A CONVENTIONAL MATRIX TRANSPOSE
//
// Parameters:
//   m - pointer to the matrix
//
// Returns:
//   pointer to the transposed matrix
//
Matrix4 *matrix4_transpose(Matrix4 *m)
{
    Matrix4 *t = matrix4_init(m->dim1, m->dim2, m->dim4, m->dim3, NULL);
    if (t == NULL)
    {
        errx(EXIT_FAILURE,
             "matrix4_transpose: failed to allocate memory for matrix\n");
    }
    for (int i = 0; i < m->dim1; i++)
        for (int j = 0; j < m->dim2; j++)
            for (int k = 0; k < m->dim3; k++)
                for (int l = 0; l < m->dim4; l++)
                    t->data[i][j][l][k] = m->data[i][j][k][l];
    return t;
}

// Function: matrix4_scalar_multiply
// ---------------------------------
// Multiplies a matrix by a scalar.
//
// Parameters:
//   m - pointer to the matrix
//   s - scalar
//
void matrix4_scalar_multiply(Matrix4 *m, float s)
{
    for (int i = 0; i < m->dim1; i++)
        for (int j = 0; j < m->dim2; j++)
            for (int k = 0; k < m->dim3; k++)
                for (int l = 0; l < m->dim4; l++)
                    m->data[i][j][k][l] *= s;
}

// Function: matrix4_element_wise_equal
// ------------------------------------
// Checks if two matrices are element-wise equal.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   true if the matrices are element-wise equal, false otherwise
//
bool matrix4_element_wise_equal(Matrix4 *m1, Matrix4 *m2)
{
    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || m1->dim3 != m2->dim3 || m1->dim4 != m2->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_element_wise_equal: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m1->dim1; i++)
        for (int j = 0; j < m1->dim2; j++)
            for (int k = 0; k < m1->dim3; k++)
                for (int l = 0; l < m1->dim4; l++)
                    if (m1->data[i][j][k][l] != m2->data[i][j][k][l])
                        return false;

    return true;
}

// Function: matrix4_get
// ---------------------
// Returns the value at the specified row and column.
//
// Parameters:
//   m - pointer to the matrix
//   dim1 - first dimension index
//   dim2 - second dimension index
//   dim3 - third dimension index
//   dim4 - fourth dimension index
//
// Returns:
//   the value at the specified row and column
//
float matrix4_get(Matrix4 *m, int dim1, int dim2, int dim3, int dim4)
{
    if (dim1 < 0 || dim1 >= m->dim1 || dim2 < 0 || dim2 >= m->dim2 || dim3 < 0 || dim3 >= m->dim3 || dim4 < 0 || dim4 >= m->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_get: index out of bounds\n");
    }
    return m->data[dim1][dim2][dim3][dim4];
}

// Function: matrix4_set
// ----------------------
// Sets the value of a matrix element.
//
// Parameters:
//   m - pointer to the matrix
//   dim1 - first dimension index
//   dim2 - second dimension index
//   dim3 - third dimension index
//   dim4 - fourth dimension index
//   value - value to set
//
void matrix4_set(Matrix4 *m, int dim1, int dim2, int dim3, int dim4, float value)
{
    if (dim1 < 0 || dim1 >= m->dim1 || dim2 < 0 || dim2 >= m->dim2 || dim3 < 0 || dim3 >= m->dim3 || dim4 < 0 || dim4 >= m->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_set: index out of bounds\n");
    }
    m->data[dim1][dim2][dim3][dim4] = value;
}

// Function: matrix4_destroy
// ------------------------
// Frees the memory allocated for a matrix.
//
// Parameters:
//   m - pointer to the matrix
//
void matrix4_destroy(Matrix4 *m)
{
    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
        {
            for (int k = 0; k < m->dim3; k++)
            {
                free(m->data[i][j][k]);
            }
            free(m->data[i][j]);
        }
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

// Function: matrix4_print
// -----------------------
// Prints a 4d matrix.
//
// Parameters:
//   m - pointer to the matrix
//
void matrix4_print(Matrix4 *m)
{

    printf("[\n");
    for (int i = 0; i < m->dim1; i++)
    {
        printf("    [\n");
        for (int j = 0; j < m->dim2; j++)
        {
            printf("        [\n");
            for (int k = 0; k < m->dim3; k++)
            {
                printf("        [");
                for (int l = 0; l < m->dim4; l++)
                {
                    printf("%f ", m->data[i][j][k][l]);
                }
                printf("]\n");
            }
            printf("        ]\n");
        }
        printf("    ]\n");
    }
    printf("]\n");
}

#pragma endregion matrix4