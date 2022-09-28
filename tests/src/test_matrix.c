
#include "../include/test_matrix.h"

int test_matrix_add()
{
    double a[2*2] = {
        1.0, 2.0,
        3.0, 4.0,
    };

    double b[2*2] = {
        4.0, 3.0,
        2.0, 1.0,
    };

    double c[2*2] = {
        5.0, 5.0,
        5.0, 5.0,
    };


    Matrix *m1 = matrix_create(2, 2, &a);
    Matrix *m2 = matrix_create(2, 2, &b);

    //matrix_print(m1);
    //matrix_print(m2);

    //Matrix *m3 = matrix_add(m1, m2);
    //Matrix *expected = matrix_create(2, 2, c);


    return print_test(0, 0, "temporary");
}
