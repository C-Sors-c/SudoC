#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <stdbool.h>
#include <math.h>

typedef struct Matrix Matrix;
typedef struct Tuple Tuple;

Matrix *matrix_init(int rows, int cols, float *datap);
Matrix *matrix_add(Matrix *m1, Matrix *m2);
Matrix *matrix_subtract(Matrix *m1, Matrix *m2);
Matrix *matrix_multiply(Matrix *m1, Matrix *m2);
void matrix_scalar_multiply(Matrix *m, float scalar);
void matrix_destroy(Matrix *m);
Tuple *matrix_dimensions(Matrix *m);
int matrix_length(Matrix *m);
float matrix_get_element(Matrix *m, int row, int col);
void matrix_set(Matrix *m, int row, int col, float value);
Matrix *matrix_transpose(Matrix *m);
bool matrix_element_wise_equal(Matrix *m1, Matrix *m2);
void matrix_print(Matrix *m);
