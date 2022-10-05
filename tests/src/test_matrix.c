
#include "../include/test_matrix.h"

float a[] = {
    1.0,
    2.0,
    3.0,
    4.0,
};

float b[] = {
    4.0,
    3.0,
    2.0,
    1.0,
};

int test_matrix_add()
{
    float c[] = {
        5.0,
        5.0,
        5.0,
        5.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);
    Matrix *m2 = matrix_init(2, 2, b);

    Matrix *m3 = matrix_add(m1, m2, NULL);
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

    float c[] = {
        -3.0,
        -1.0,
        1.0,
        3.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);
    Matrix *m2 = matrix_init(2, 2, b);

    Matrix *m3 = matrix_subtract(m1, m2, NULL);
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

    float c[] = {
        8.0,
        5.0,
        20.0,
        13.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);
    Matrix *m2 = matrix_init(2, 2, b);

    Matrix *m3 = matrix_multiply(m1, m2, NULL);
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

    float c[] = {
        2.0,
        4.0,
        6.0,
        8.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);

    matrix_scalar_multiply(m1, 2.0);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m1, expected);

    matrix_destroy(m1);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_multiply_scalar");
}

int test_matrix_transpose()
{

    float c[] = {
        1.0,
        3.0,
        2.0,
        4.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);

    Matrix *m2 = matrix_transpose(m1);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m2, expected);

    matrix_destroy(m1);
    matrix_destroy(m2);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_transpose");
}

int test_matrix4_add()
{
    float c[] = {
        5.0,
        5.0,
        5.0,
        5.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);
    Matrix4 *m2 = matrix4_init(1, 1, 2, 2, b);

    Matrix4 *m3 = matrix4_add(m1, m2, NULL);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m3, expected);

    matrix4_destroy(m1);
    matrix4_destroy(m2);
    matrix4_destroy(m3);
    matrix4_destroy(expected);

    return print_test(diff, true, "test_matrix4_add");
}

int test_matrix4_subtract()
{

    float c[] = {
        -3.0,
        -1.0,
        1.0,
        3.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);
    Matrix4 *m2 = matrix4_init(1, 1, 2, 2, b);

    Matrix4 *m3 = matrix4_subtract(m1, m2, NULL);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m3, expected);

    matrix4_destroy(m1);
    matrix4_destroy(m2);
    matrix4_destroy(m3);
    matrix4_destroy(expected);

    return print_test(diff, true, "test_matrix4_subtract");
}

int test_matrix4_multiply_scalar()
{
    float c[] = {
        2.0,
        4.0,
        6.0,
        8.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);

    matrix4_scalar_multiply(m1, 2.0);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m1, expected);

    matrix4_destroy(m1);
    matrix4_destroy(expected);

    return print_test(diff, true, "test_matrix4_multiply_scalar");
}

int test_matrix4_transpose()
{
    float c[] = {
        1.0,
        3.0,
        2.0,
        4.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);

    Matrix4 *m2 = matrix4_transpose(m1);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m2, expected);

    matrix4_destroy(m1);
    matrix4_destroy(m2);
    matrix4_destroy(expected);

    return print_test(diff, true, "test_matrix4_transpose");
}

// TODO: more tests on the new matrix functions