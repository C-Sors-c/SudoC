
#include "../include/test_matrix.h"
    
double a[] = {
    1.0, 2.0,
    3.0, 4.0,
};

double b[] = {
    4.0, 3.0,
    2.0, 1.0,
};

int test_matrix_add()
{
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

int test_matrix_subtract()
{

    double c [] = {
        -3.0, -1.0,
        1.0, 3.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);
    Matrix *m2 = matrix_init(2, 2, b);
    
    Matrix *m3 = matrix_subtract(m1, m2);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m3, expected);

    matrix_destroy(m1);
    matrix_destroy(m2);
    matrix_destroy(m3);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_subtract");
}

int test_matrix_multiply()
{

    double c[] = {
        8.0, 5.0,
        20.0, 13.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);
    Matrix *m2 = matrix_init(2, 2, b);
    
    Matrix *m3 = matrix_multiply(m1, m2);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m3, expected);

    matrix_destroy(m1);
    matrix_destroy(m2);
    matrix_destroy(m3);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_multiply");
}

int test_matrix_multiply_scalar()
{

    double c[] = {
        2.0, 4.0,
        6.0, 8.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);
    
    matrix_scalar_multiply(m1, 2.0);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m1, expected);

    matrix_destroy(m1);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_multiply_scalar");
}