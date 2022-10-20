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

    m->size = dim1 * dim2;
    if (datap != NULL)
    {
        // copy provided data to the new matrix
        m->data = malloc(m->size * sizeof(m->data));
        for (int i = 0; i < m->size; i++)
            m->data[i] = datap[i];
    }
    else
    {
        // fill the data of the matrix with zeroes
        m->data = calloc(m->size, sizeof(float));
    }
    return m;
}

// Function: matrix_copy
// -----------------------
// Copies the data from a matrix to a new one
//
// Parameters:
//   m - a pointer to the matrix
//   dst - a pointer to the destination matrix
//

Matrix *matrix_copy(Matrix *m, Matrix *dst)
{
    if (dst == NULL)
        dst = matrix_init(m->dim1, m->dim2, NULL);

    if (m->dim1 != dst->dim1 || m->dim2 != dst->dim2)
    {
        matrix_print(m);
        matrix_print(dst);
        errx(EXIT_FAILURE, "Error: matrix_copy: dimensions mismatch");
    }

    for (int i = 0; i < m->dim1 * m->dim2; i++)
        dst->data[i] = m->data[i];

    return dst;
}

// Function: matrix_zero
// -----------------------
// Resets the matrix to zero values
//
// Parameters:
//   m - a pointer to the matrix
//

void matrix_zero(Matrix *m)
{
    for (int i = 0; i < m->size; i++)
        m->data[i] = 0;
}

// Function: m_get
// -----------------------
// Gets a value from a row major matrix
//
// Parameters:
//   m - a pointer to the matrix
//   dim1 - number of row
//   dim2 - number of column
//

float m_get(Matrix *m, int i, int j)
{
    if (i < 0 || i >= m->dim1 || j < 0 || j >= m->dim2)
    {
        errx(EXIT_FAILURE, "m_get: out of range");
    }

    return m->data[i * m->dim2 + j];
}

// Function: m_set
// ----------------------
// Sets a value into a row major matrix
//
// Parameters:
//   m - a pointer to the matrix
//   dim1 - number of row
//   dim2 - number of column
//

void m_set(Matrix *m, int dim1, int dim2, float value)
{
    if (dim1 < 0 || dim1 >= m->dim1 || dim2 < 0 || dim2 >= m->dim2)
    {
        errx(EXIT_FAILURE, "m_set: out of range");
    }

    m->data[dim1 * m->dim2 + dim2] = value;
}

// Function: matrix_add
// --------------------
// Adds two matrices and returns the result.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_add(Matrix *m1, Matrix *m2, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m1->dim1, m1->dim2, NULL);
    }

    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2)
    {
        printf("m1->dim1: %d m1->dim2: %d m2->dim1: %d m2->dim2: %d dst->dim1: %d dst->dim2: %d\n", m1->dim1, m1->dim2, m2->dim1, m2->dim2, dst->dim1, dst->dim2);
        errx(EXIT_FAILURE, "matrix_add: matrix dimensions do not match\n");
    }

    for (int i = 0; i < dst->dim1; i++)
    {
        for (int j = 0; j < dst->dim2; j++)
        {
            dst->data[i * dst->dim2 + j] = m1->data[i * dst->dim2 + j] + m2->data[i * dst->dim2 + j];
        }
    }
    return dst;
}

// Function: matrix_add_bias
// --------------------
// Adds a bias matrix to a matrix and returns the result.
//
// Parameters:
//   m1 - pointer to the matrix
//   m2 - pointer to the bias matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_add_bias(Matrix *m1, Matrix *m2, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m1->dim1, m1->dim2, NULL);
    }

    if (m1->dim2 != m2->dim2 || m1->dim1 != dst->dim1 || m1->dim2 != dst->dim2)
    {
        printf("m1->dim1: %d m1->dim2: %d m2->dim1: %d m2->dim2: %d dst->dim1: %d dst->dim2: %d\n", m1->dim1, m1->dim2, m2->dim1, m2->dim2, dst->dim1, dst->dim2);
        errx(EXIT_FAILURE, "matrix_add_bias: matrix dimensions do not match\n");
    }

    for (int j = 0; j < dst->dim2; j++)
    {
        for (int i = 0; i < dst->dim1; i++)
        {
            dst->data[i * dst->dim2 + j] = dst->data[i * dst->dim2 + j] + m2->data[j];
        }
    }
    return dst;
}

// Function: matrix_sum_rows
// --------------------
// Sums the rows of a matrix and returns the result.
//
// Parameters:
//   m1 - pointer to the matrix
//   dst - pointer to the destination matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_sum_rows(Matrix *m1, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(1, m1->dim2, NULL);
    }

    if (m1->dim2 != dst->dim2)
    {
        printf("m1->dim1: %d m1->dim2: %d dst->dim1: %d dst->dim2: %d\n", m1->dim1, m1->dim2, dst->dim1, dst->dim2);
        errx(EXIT_FAILURE, "matrix_sum_rows: matrix dimensions do not match\n");
    }

    for (int j = 0; j < dst->dim2; j++)
    {
        dst->data[j] = 0.0;
        for (int i = 0; i < dst->dim1; i++)
        {
            dst->data[j] += m1->data[i * m1->dim2 + j];
        }
    }
    return dst;
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

Matrix *matrix_subtract(Matrix *m1, Matrix *m2, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m1->dim1, m1->dim2, NULL);
    }

    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || dst->dim1 != m1->dim1 || dst->dim2 != m1->dim2)
    {
        errx(EXIT_FAILURE, "matrix_subtract: matrix dimensions do not match\n");
    }

    for (int i = 0; i < dst->dim1; i++)
    {
        for (int j = 0; j < dst->dim2; j++)
        {
            dst->data[i * dst->dim2 + j] = m1->data[i * dst->dim2 + j] - m2->data[i * dst->dim2 + j];
        }
    }
    return dst;
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

Matrix *matrix_multiply(Matrix *m1, Matrix *m2, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m1->dim1, m2->dim2, NULL);
    }

    if (m1->dim2 != m2->dim1 || dst->dim1 != m1->dim1 || dst->dim2 != m2->dim2)
    {
        printf("m1->dim2 != m2->dim1: %d != %d \n", m1->dim2, m2->dim1);
        printf("dst->dim1 != m1->dim1: %d != %d \n", dst->dim1, m1->dim1);
        printf("dst->dim2 != m2->dim2: %d != %d \n", dst->dim2, m2->dim2);

        printf("m1->dim1: %d m1->dim2: %d m2->dim1: %d m2->dim2: %d dst->dim1: %d dst->dim2: %d\n", m1->dim1, m1->dim2, m2->dim1, m2->dim2, dst->dim1, dst->dim2);
        errx(EXIT_FAILURE, "matrix_multiply: matrix dimensions do not match, expected output to be (%i, %i)\n", m1->dim1, m2->dim2);
    }

    for (int i = 0; i < dst->dim1; i++)
    {
        for (int j = 0; j < dst->dim2; j++)
        {
            dst->data[i * dst->dim2 + j] = 0.0;
            for (int k = 0; k < m1->dim2; k++)
            {
                dst->data[i * dst->dim2 + j] += m1->data[i * m1->dim2 + k] * m2->data[k * m2->dim2 + j];
            }
        }
    }
    return dst;
}

// Function: matrix_multiply_scalar
// --------------------------------
// Multiplies a matrix by a scalar.
//
// Parameters:
//   m - pointer to the matrix
//   scalar - the scalar
//

void matrix_multiply_scalar(Matrix *m, float s)
{
    for (int i = 0; i < m->dim1; i++)
        for (int j = 0; j < m->dim2; j++)
            m->data[i * m->dim2 + j] *= s;
}

// Function: matrix_map_function
// -----------------------------
// Applies a function to each element of a matrix.
//
// Parameters:
//   m - pointer to the matrix
//   f - the function
//

void matrix_map_function(Matrix *m, float(f)(float))
{
    for (int i = 0; i < m->dim1; i++)
        for (int j = 0; j < m->dim2; j++)
            m->data[i * m->dim2 + j] = f(m->data[i * m->dim2 + j]);
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
    free(m->data);
    free(m);
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
            t->data[j * m->dim2 + i] = m->data[i * m->dim2 + j];
        }
    }
    return t;
}

// Function: matrix_elementwise_multiply
// -------------------------------------
// Multiplies two matrices elementwise.
//
// Parameters:
//   m1 - pointer to the first matrix
//   m2 - pointer to the second matrix
//   dst - pointer to the destination matrix
//
// Returns:
//   a pointer to the result matrix
//

Matrix *matrix_elementwise_multiply(Matrix *m1, Matrix *m2, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m1->dim1, m1->dim2, NULL);
    }

    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || dst->dim1 != m1->dim1 || dst->dim2 != m1->dim2)
    {
        errx(EXIT_FAILURE, "matrix_elementwise_multiply: matrix dimensions do not match\n");
    }

    for (int i = 0; i < dst->dim1; i++)
    {
        for (int j = 0; j < dst->dim2; j++)
        {
            dst->data[i * m1->dim2 + j] = m1->data[i * m1->dim2 + j] * m2->data[i * m2->dim2 + j];
        }
    }
    return dst;
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

    for (int i = 0; i < m1->size; i++)
        if (m1->data[i] != m2->data[i])
            return false;

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
    printf("dim1:%i, dim2:%i\n", m->dim1, m->dim2);
    for (int i = 0; i < m->dim1; i++)
    {
        printf("[");
        for (int j = 0; j < m->dim2 - 1; j++)
        {
            printf("%f ", m->data[i * m->dim2 + j]);
        }
        printf("%f]\n", m->data[i * m->dim2 + m->dim2 - 1]);
    }
}

#pragma endregion matrix

#pragma region matrix4

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

    m->size = dim1 * dim2 * dim3 * dim4;

    if (datap != NULL)
    {
        m->data = calloc(m->size, sizeof(float));
        for (int i = 0; i < m->size; i++)
        {
            // initialize data from datap
            m->data[i] = datap[i];
        }
    }
    else
    {
        m->data = calloc(m->size, sizeof(float));
        if (m->data == NULL)
            malloc_error();
    }

    return m;
}

// Function: matrix4_zero
// ----------------------
// Reset all values in a 4-dimensional matrix to zero.
//
// Parameters:
//   m - pointer to the matrix
//

void matrix4_zero(Matrix4 *m)
{
    for (int i = 0; i < m->size; i++)
        m->data[i] = 0.0f;
}

// Function: matrix4_copy
// ----------------------
// Copies a 4-dimensional matrix.
//
// Parameters:
//   m - pointer to the matrix
//   dst - pointer to the destination matrix
//

void matrix4_copy(Matrix4 *m, Matrix4 *dst)
{
    if (m->dim1 != dst->dim1 || m->dim2 != dst->dim2 || m->dim3 != dst->dim3 || m->dim4 != dst->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_copy: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m->size; i++)
        dst->data[i] = m->data[i];
}

// Function: m4_get
// ----------------
// Gets the value at a specific index in a 4-dimensional matrix.
//
// Parameters:
//   m - pointer to the matrix
//   i - index of dimension 1
//   j - index of dimension 2
//   k - index of dimension 3
//   l - index of dimension 4
//

float m4_get(Matrix4 *m, int i, int j, int k, int l)
{
    if (i < 0 || i >= m->dim1 || j < 0 || j >= m->dim2 || k < 0 || k >= m->dim3 || l < 0 || l >= m->dim4)
    {
        errx(EXIT_FAILURE, "m4_get: index out of bounds\n");
    }

    return m->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + k * m->dim4 + l];
}

// Function: m4_set
// ----------------
// Sets a value in a 4-dimensional matrix.
//
// Parameters:
//   m - pointer to the matrix
//   i - index of dimension 1
//   j - index of dimension 2
//   k - index of dimension 3
//   l - index of dimension 4
//   value - value to set
//

void m4_set(Matrix4 *m, int i, int j, int k, int l, float value)
{
    if (i < 0 || i >= m->dim1 || j < 0 || j >= m->dim2 || k < 0 || k >= m->dim3 || l < 0 || l >= m->dim4)
    {
        errx(EXIT_FAILURE, "m4_set: index out of bounds\n");
    }

    m->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + k * m->dim4 + l] = value;
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
Matrix4 *matrix4_add(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst)
{
    if (dst == NULL)
    {
        dst = matrix4_init(m1->dim1, m1->dim2, m1->dim3, m1->dim4, NULL);
    }

    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || m1->dim3 != m2->dim3 || m1->dim4 != m2->dim4 ||
        m1->dim1 != dst->dim1 || m1->dim2 != dst->dim2 || m1->dim3 != dst->dim3 || m1->dim4 != dst->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_add: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m1->size; i++)
        dst->data[i] = m1->data[i] + m2->data[i];

    return dst;
}

// Function: matrix4_add_bias
// ---------------------------------
// Adds a 2d matrix to a 4-dimensional matrix element-wise.
//
// Parameters:
//   m1 - pointer to the 4-dimensional matrix
//   m2 - pointer to the 2-dimensional matrix
//   dst - pointer to the destination matrix
//
// Returns:
//   pointer to the resulting matrix
//

Matrix4 *matrix4_add_bias(Matrix4 *m1, Matrix *bias, Matrix4 *dst)
{
    if (dst == NULL)
    {
        dst = matrix4_init(m1->dim1, m1->dim2, m1->dim3, m1->dim4, NULL);
    }

    if (m1->dim1 != dst->dim1 || m1->dim2 != dst->dim2 || m1->dim3 != dst->dim3 || m1->dim4 != dst->dim4 || m1->dim2 != bias->dim1)
    {
        errx(EXIT_FAILURE, "matrix4_sum_bias: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m1->dim1; i++)
    {
        for (int j = 0; j < m1->dim2; j++)
        {
            for (int k = 0; k < m1->dim3; k++)
            {
                for (int l = 0; l < m1->dim4; l++)
                {
                    dst->data[i * m1->dim2 * m1->dim3 * m1->dim4 + j * m1->dim3 * m1->dim4 + k * m1->dim4 + l] =
                        m1->data[i * m1->dim2 * m1->dim3 * m1->dim4 + j * m1->dim3 * m1->dim4 + k * m1->dim4 + l] + bias->data[j];
                }
            }
        }
    }

    return dst;
}

// Function: matrix4_flatten
// -------------------------
// Flattens a 4-dimensional matrix into a 2-dimensional matrix.
//
// Parameters:
//   m - pointer to the matrix
//
// Returns:
//   pointer to the resulting matrix
//

Matrix *matrix4_flatten(Matrix4 *m, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m->dim1 * m->dim2 * m->dim3, m->dim4, NULL);
    }

    if (m->dim1 * m->dim2 * m->dim3 != dst->dim1 || m->dim4 != dst->dim2)
    {
        errx(EXIT_FAILURE, "matrix4_flatten: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m->size; i++)
        dst->data[i] = m->data[i];

    return dst;
}

// Function: matrix4_unflatten
// ---------------------------
// Unflattens a 2-dimensional matrix into a 4-dimensional matrix.
//
// Parameters:
//   m - pointer to the matrix
//
// Returns:
//   pointer to the resulting matrix
//

Matrix4 *matrix4_unflatten(Matrix *m, Matrix4 *dst)
{
    if (dst == NULL)
    {
        dst = matrix4_init(m->dim1, m->dim2 / (dst->dim3 * dst->dim4), dst->dim3, dst->dim4, NULL);
    }

    for (int i = 0; i < m->size; i++)
        dst->data[i] = m->data[i];

    return dst;
}

// Function: matrix4_sum_rows
// ---------------------------------
// Calculates the bias gradient from 4d delta matrix.
//
// Parameters:
//   m1 - pointer to the 4-dimensional matrix
//   dst - pointer to the destination matrix
//
// Returns:
//   pointer to the resulting matrix
//

Matrix *matrix4_sum_rows(Matrix4 *m, Matrix *dst)
{
    if (dst == NULL)
    {
        dst = matrix_init(m->dim2, 1, NULL);
    }

    if (m->dim2 != dst->dim1)
    {
        errx(EXIT_FAILURE, "matrix4_sum_rows: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m->dim1; i++)
    {
        for (int j = 0; j < m->dim2; j++)
        {
            for (int k = 0; k < m->dim3; k++)
            {
                for (int l = 0; l < m->dim4; l++)
                {
                    dst->data[j] += m->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + k * m->dim4 + l];
                }
            }
        }
    }

    return dst;
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
Matrix4 *matrix4_subtract(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst)
{
    if (dst == NULL)
    {
        dst = matrix4_init(m1->dim1, m1->dim2, m1->dim3, m1->dim4, NULL);
    }

    if (m1->dim1 != m2->dim1 || m1->dim2 != m2->dim2 || m1->dim3 != m2->dim3 || m1->dim4 != m2->dim4 ||
        m1->dim1 != dst->dim1 || m1->dim2 != dst->dim2 || m1->dim3 != dst->dim3 || m1->dim4 != dst->dim4)
    {
        errx(EXIT_FAILURE, "matrix4_subtract: matrix dimensions do not match\n");
    }

    for (int i = 0; i < m1->size; i++)
        dst->data[i] = m1->data[i] - m2->data[i];

    return dst;
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
    {
        for (int j = 0; j < m->dim2; j++)
        {
            for (int k = 0; k < m->dim3; k++)
            {
                for (int l = 0; l < m->dim4; l++)
                {
                    t->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + k * m->dim4 + l] =
                        m->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + l * m->dim3 + k];
                }
            }
        }
    }

    // change dimensions
    int tmp = t->dim3;
    t->dim3 = t->dim4;
    t->dim4 = tmp;

    return t;
}

// Function: matrix4_convolve
// --------------------------
// Convolves a 4D matrix with a 4D kernel.
//
// Parameters:
//   weights - pointer to the weights of shape: (out_channels, in_channels, kernel_height, kernel_width)
//   input - pointer to the matrix of shape: (batch_size, in_channels, height, width)
//   dst - pointer to the destination matrix
//   stride - stride of the convolution
//   padding - padding of the convolution
// Returns:
//   pointer to the resulting matrix

Matrix4 *matrix4_convolve(Matrix4 *weights, Matrix4 *input, Matrix4 *dst, int stride, int padding)
{

    int batch_size = input->dim1;
    int in_channels = input->dim2;
    int height = input->dim3;
    int width = input->dim4;

    int out_channels = weights->dim1;
    int kernel_height = weights->dim3;
    int kernel_width = weights->dim4;

    int out_height = (height + 2 * padding - kernel_height) / stride + 1;
    int out_width = (width + 2 * padding - kernel_width) / stride + 1;

    if (dst == NULL)
    {
        dst = matrix4_init(batch_size, out_channels, out_height, out_width, NULL);
    }

    if (weights->dim2 != in_channels)
    {
        errx(EXIT_FAILURE, "matrix4_convolve: input channels do not match\n");
    }

    if (dst->dim1 != batch_size || dst->dim2 != out_channels || dst->dim3 != out_height || dst->dim4 != out_width)
    {
        errx(EXIT_FAILURE, "matrix4_convolve: output dimensions do not match\n");
    }

    // optimized convolution using strides and padding

    for (int i = 0; i < batch_size; i++)
    {
        for (int j = 0; j < out_channels; j++)
        {
            for (int k = 0; k < out_height; k++)
            {
                for (int l = 0; l < out_width; l++)
                {
                    for (int m = 0; m < in_channels; m++)
                    {
                        for (int n = 0; n < kernel_height; n++)
                        {
                            for (int o = 0; o < kernel_width; o++)
                            {
                                int x = k * stride + n - padding;
                                int y = l * stride + o - padding;

                                if (x >= 0 && x < height && y >= 0 && y < width)
                                {
                                    dst->data[i * out_channels * out_height * out_width + j * out_height * out_width + k * out_width + l] +=
                                        weights->data[j * in_channels * kernel_height * kernel_width + m * kernel_height * kernel_width + n * kernel_width + o] *
                                        input->data[i * in_channels * height * width + m * height * width + x * width + y];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return dst;
}

// Function: matrix4_convolve_transpose
// ------------------------------------
// Convolves a 4D matrix with a 180 degrees transposed 4D kernel.
//
// Parameters:
//   weights - pointer to the weights of shape: (out_channels, in_channels, kernel_height, kernel_width)
//   input - pointer to the matrix of shape: (batch_size, in_channels, height, width)
//   dst - pointer to the destination matrix
//   stride - stride of the convolution
//   padding - padding of the convolution
// Returns:
//   pointer to the resulting matrix

Matrix4 *matrix4_convolve_transpose(Matrix4 *weights, Matrix4 *input, Matrix4 *dst, int stride, int padding)
{
    // Transpose by 180 degrees the weights
    Matrix4 *weights_t = matrix4_transpose(matrix4_transpose(weights));
    return matrix4_convolve(weights_t, input, dst, stride, padding);
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
    for (int i = 0; i < m->size; i++)
        m->data[i] *= s;
}

// Function: matrix4_map_function
// ------------------------------
// Applies a function to each element of a matrix.
//
// Parameters:
//   m - pointer to the matrix
//   f - the function
//
void matrix4_map_function(Matrix4 *m, float(f)(float))
{
    for (int i = 0; i < m->size; i++)
        m->data[i] = f(m->data[i]);
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

    for (int i = 0; i < m1->size; i++)
        if (m1->data[i] != m2->data[i])
            return false;

    return true;
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
                for (int l = 0; l < m->dim4 - 1; l++)
                {
                    printf("%f ", m->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + k * m->dim4 + l]);
                }
                printf("%f]\n", m->data[i * m->dim2 * m->dim3 * m->dim4 + j * m->dim3 * m->dim4 + k * m->dim4 + m->dim4 - 1]);
            }
            printf("        ]\n");
        }
        printf("    ]\n");
    }
    printf("]\n");
}

#pragma endregion matrix4