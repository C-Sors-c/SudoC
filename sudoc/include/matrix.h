#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <stdbool.h>
#include <math.h>

typedef struct
{
    int x;
    int y;
} Tupple;

Tupple T(int x, int y);

// 2D matrix utils
struct Matrix
{
    int dim1; // number of rows
    int dim2; // number of columns
    int size; // size of 1D array
    float *data;
};

typedef struct Matrix Matrix;
#define MAT(matrix, i, j) (matrix->data[(i)*matrix->dim2 + (j)])
Matrix *matrix_init(int dim1, int dim2, float *datap);
Matrix *matrix_copy(Matrix *m, Matrix *dst);
void matrix_zero(Matrix *m);
float m_get(Matrix *m, int i, int j);
void m_set(Matrix *m, int dim1, int dim2, float value);

Matrix *matrix_add(Matrix *m1, Matrix *m2, Matrix *dst);
int* matrix_argmax(Matrix *m);
Matrix *matrix_add_bias(Matrix *m1, Matrix *m2, Matrix *dst);
Matrix *matrix_sum_rows(Matrix *m1, Matrix *dst);
Matrix *matrix_subtract(Matrix *m1, Matrix *m2, Matrix *dst);
Matrix *matrix_multiply(Matrix *m1, Matrix *m2, Matrix *dst);
Matrix *matrix_transpose(Matrix *m);
Matrix *matrix_elementwise_multiply(Matrix *m1, Matrix *m2, Matrix *dst);
void matrix_multiply_scalar(Matrix *m, float s);
void matrix_map_function(Matrix *m, float (*func)(float));
bool matrix_element_wise_equal(Matrix *m1, Matrix *m2);
void matrix_destroy(Matrix *m);
void matrix_print(Matrix *m);
void matrix_printshape(Matrix *m);
float matrix_det(Matrix *m);
Matrix *matrix_inverse(Matrix *m);
Matrix *matrix_transformation(const Tupple *src, const Tupple *dst);

// 4D matrix utils
struct Matrix4
{
    int dim1;
    int dim2;
    int dim3;
    int dim4;
    int size;
    float *data;
};

typedef struct Matrix4 Matrix4;
#define MAT4(matrix, i, j, k, l) (matrix->data[(i)*matrix->dim2 * matrix->dim3 * matrix->dim4 + (j)*matrix->dim3 * matrix->dim4 + (k)*matrix->dim4 + (l)])
Matrix4 *matrix4_init(int dim1, int dim2, int dim3, int dim4, float *datap);
Matrix4 *matrix4_copy(Matrix4 *m, Matrix4 *dst);
void matrix4_zero(Matrix4 *m);
float m4_get(Matrix4 *m, int dim1, int dim2, int dim3, int dim4);
void m4_set(Matrix4 *m, int dim1, int dim2, int dim3, int dim4, float value);

Matrix4 *matrix4_add(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst);
Matrix4 *matrix4_subtract(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst);
Matrix4 *matrix4_transpose(Matrix4 *m);
Matrix4 *matrix4_convolve(Matrix4 *weights, Matrix4 *input, Matrix4 *dst, int stride, int padding);
Matrix4 *matrix4_convolve_transpose(Matrix4 *weights, Matrix4 *input, Matrix4 *dst, int stride, int padding);
Matrix4 *matrix4_grad_input_convolve(Matrix4 *weights, Matrix4 *grad_output, Matrix4 *dst, int stride, int padding);
Matrix4 *matrix4_add_bias(Matrix4 *m1, Matrix *bias, Matrix4 *dst);
Matrix *matrix4_flatten(Matrix4 *m, Matrix *dst);
Matrix4 *matrix4_unflatten(Matrix *m, Matrix4 *dst);
Matrix *matrix4_sum_channels(Matrix4 *m1, Matrix *dst);
Matrix4 *matrix4_elementwise_multiply(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst);
void matrix4_multiply_scalar(Matrix4 *m, float s);
void matrix4_map_function(Matrix4 *m, float (*func)(float));
bool matrix4_element_wise_equal(Matrix4 *m1, Matrix4 *m2);
void matrix4_destroy(Matrix4 *m);
void matrix4_print(Matrix4 *m);
void matrix4_printshape(Matrix4 *m);