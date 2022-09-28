
#include "../include/test_matrix.h"

int test_matrix_add(Matrix *m1, Matrix *m2)
{
    Matrix *m3 = matrix_add(m1, m2);
    return print_test(0, 0, "temporary");
}
