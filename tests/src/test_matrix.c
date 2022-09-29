
#include "../include/test_matrix.h"

int test_matrix_add()
{
    double a[] = {
        1.0, 2.0,
        3.0, 4.0,
    };

    double b[] = {
        4.0, 3.0,
        2.0, 1.0,
    };

    double c[] = {
        5.0, 5.0,
        5.0, 5.0,
    };


    Matrix *m1 = matrix_init(2, 2, a);
    Matrix *m2 = matrix_init(2, 2, b);
    
    Matrix *m3 = matrix_add(m1, m2);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m3, expected);

    matrix_destroy(m1);
    matrix_destroy(m2);
    matrix_destroy(m3);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_add");
}
