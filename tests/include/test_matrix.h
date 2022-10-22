#pragma once

#include "../../sudoc/include/utils.h"
#include "../../sudoc/include/matrix.h"

int test_matrix_add();
int test_matrix_subtract();
int test_matrix_multiply();
int test_matrix_multiply_scalar();
int test_matrix_transpose();
int test_matrix_map_function();

int test_matrix4_init();
int test_matrix4_add();
int test_matrix4_subtract();
int test_matrix4_multiply();
int test_matrix4_multiply_scalar();
int test_matrix4_transpose();
int test_matrix4_map_function();
int test_matrix4_convolve();
int test_matrix4_add_bias();
int test_matrix4_sum_rows();
