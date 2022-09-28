#pragma once

typedef struct Matrix Matrix;
typedef struct Tuple Tuple;

Matrix *matrix_create(int rows, int cols, double **datap);
Matrix *matrix_add(Matrix *m1, Matrix *m2);
Matrix *matrix_substract(Matrix *m1, Matrix *m2);
void matrix_print(Matrix *m);
Matrix *matrix_multiply(Matrix *m1, Matrix *m2);
void matrix_destroy(Matrix *m);
Tuple *matrix_dimensions(Matrix *m);
int matrix_length(Matrix *m);
double matrix_get_element(Matrix *m, int row, int col);
void matrix_set_element(Matrix *m, int row, int col, double value);
Matrix *matrix_transpose(Matrix *m);
