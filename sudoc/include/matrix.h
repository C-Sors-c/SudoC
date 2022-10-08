#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <stdbool.h>
#include <math.h>

// 2D matrix utils
struct Matrix
{
    int dim1; // number of rows
    int dim2; // number of columns
    float **data;
};

typedef struct Matrix Matrix;
Matrix *matrix_init(int dim1, int dim2, float *datap);
void matrix_copy(Matrix *m, Matrix *dst);
void matrix_zero(Matrix *m);

Matrix *matrix_add(Matrix *m1, Matrix *m2, Matrix *dst);
Matrix *matrix_subtract(Matrix *m1, Matrix *m2, Matrix *dst);
Matrix *matrix_multiply(Matrix *m1, Matrix *m2, Matrix *dst);
Matrix *matrix_transpose(Matrix *m);
void matrix_scalar_multiply(Matrix *m, float s);
void matrix_map_function(Matrix *m, float (*func)(float));
bool matrix_element_wise_equal(Matrix *m1, Matrix *m2);
float matrix_get(Matrix *m, int row, int col);
void matrix_set(Matrix *m, int row, int col, float value);
void matrix_destroy(Matrix *m);
void matrix_print(Matrix *m);

// 4D matrix utils
struct Matrix4
{
    int dim1;
    int dim2;
    int dim3;
    int dim4;
    float ****data;
};

typedef struct Matrix4 Matrix4;
Matrix4 *matrix4_init(int dim1, int dim2, int dim3, int dim4, float *datap);
void matrix4_copy(Matrix4 *m, Matrix4 *dst);
void matrix4_zero(Matrix4 *m);

Matrix4 *matrix4_add(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst);
Matrix4 *matrix4_subtract(Matrix4 *m1, Matrix4 *m2, Matrix4 *dst);
Matrix4 *matrix4_transpose(Matrix4 *m);
Matrix4 *matrix4_convolve(Matrix4 *weights, Matrix4 *input, Matrix4 *dst, int stride, int padding);
Matrix4 *matrix4_convolve_transpose(Matrix4 *weights, Matrix4 *input, Matrix4 *dst, int stride, int padding);
Matrix4 *matrix4_add_bias(Matrix4 *m1, Matrix *bias, Matrix4 *dst);
Matrix *matrix4_flatten(Matrix4 *m, Matrix *dst);
Matrix4 *matrix4_unflatten(Matrix *m, Matrix4 *dst);
Matrix *matrix4_sum_bias(Matrix4 *m1, Matrix *dst);
void matrix4_scalar_multiply(Matrix4 *m, float s);
void matrix4_map_function(Matrix4 *m, float (*func)(float));
bool matrix4_element_wise_equal(Matrix4 *m1, Matrix4 *m2);
float matrix4_get(Matrix4 *m, int row, int col, int dim3, int dim4);
void matrix4_set(Matrix4 *m, int row, int col, int dim3, int dim4, float value);
void matrix4_destroy(Matrix4 *m);
void matrix4_print(Matrix4 *m);
