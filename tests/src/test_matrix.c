
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

// square each element: used in some tests
float square(float x)
{
    return x * x;
}

#pragma region matrix_tests

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

    matrix_multiply_scalar(m1, 2.0);
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

int test_matrix_map_function()
{
    float c[] = {
        1.0,
        4.0,
        9.0,
        16.0,
    };

    Matrix *m1 = matrix_init(2, 2, a);

    matrix_map_function(m1, square);
    Matrix *expected = matrix_init(2, 2, c);

    bool diff = matrix_element_wise_equal(m1, expected);

    matrix_destroy(m1);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix_map_function");
}

#pragma endregion matrix_tests

#pragma region matrix_4_tests

int test_matrix4_init()
{
    Matrix4 *m = matrix4_init(5, 1000, 1000, 3, NULL);
    matrix4_destroy(m);
    return print_test(true, true, "test_matrix4_init");
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

int test_matrix4_map_function()
{
    float c[] = {
        1.0,
        4.0,
        9.0,
        16.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);

    matrix4_map_function(m1, square);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m1, expected);

    matrix4_destroy(m1);
    matrix4_destroy(expected);

    return print_test(diff, true, "test_matrix4_map_function");
}

int test_matrix4_convolve()
{
    float input[] = {
        1.0,
        2.0,
        3.0,
        4.0,
        5.0,
        6.0,
        7.0,
        8.0,
        9.0,
    };

    float kernel[] = {
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
    };

    float c[] = {
        1.0,
        5.0,
        11.0,
        28.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, kernel);
    Matrix4 *m2 = matrix4_init(1, 1, 3, 3, input);

    // stride is set to 1 and padding to 0
    Matrix4 *m3 = matrix4_convolve(m1, m2, NULL, 2, 1);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m3, expected);

    matrix4_destroy(m1);
    matrix4_destroy(m2);
    matrix4_destroy(m3);
    matrix4_destroy(expected);

    return print_test(diff, true, "test_matrix4_convolve");
}

int test_matrix4_add_bias()
{
    float bias[] = {
        4.0,
    };

    float c[] = {
        5.0,
        6.0,
        7.0,
        8.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);
    Matrix *m2 = matrix_init(1, 1, bias);

    Matrix4 *m3 = matrix4_add_bias(m1, m2, NULL);
    Matrix4 *expected = matrix4_init(1, 1, 2, 2, c);

    bool diff = matrix4_element_wise_equal(m3, expected);

    matrix4_destroy(m1);
    matrix4_destroy(m3);
    matrix4_destroy(expected);
    matrix_destroy(m2);

    return print_test(diff, true, "test_matrix4_add_bias");
}

int test_matrix4_sum_rows()
{

    float c[] = {
        10.0,
    };

    Matrix4 *m1 = matrix4_init(1, 1, 2, 2, a);
    Matrix *m2 = matrix4_sum_rows(m1, NULL);
    Matrix *expected = matrix_init(1, 1, c);

    bool diff = matrix_element_wise_equal(m2, expected);

    matrix4_destroy(m1);
    matrix_destroy(m2);
    matrix_destroy(expected);

    return print_test(diff, true, "test_matrix4_sum_rows");
}

// TODO: more tests on the new matrix functions

#pragma endregion matrix_4_tests