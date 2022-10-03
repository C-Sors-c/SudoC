#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../sudoc/libs/include/utils.h"
#include "include/test_add.h"
#include "include/test_cv.h"

#include "include/test_matrix.h"
#include "include/test_sdl.h"

//lists of all dummy tests
int (*test_dummy[])() = {
    test_add_positive,
    test_add_negative,
};

//lists of all cv tests
int (*test_cv[])() = {
    test_cv_init_image_rgb,
};

// lists of all matrix tests
int (*tests_matrix[])() = {
    test_matrix_add,
    test_matrix_subtract,
    test_matrix_multiply,
    test_matrix_multiply_scalar,
    test_matrix_transpose,
    test_matrix4_add,
    test_matrix4_subtract,
    test_matrix4_multiply,
    test_matrix4_multiply_scalar,
    test_matrix4_transpose,
};

int (*tests_sdl[])() = {
    // test_sdl_surface_to_array_red,
    // test_to_green,
};

int main()
{
    // start timer
    clock_t begin = clock();

    // run all tests
    int failed = 0;
    int test_count = 0;

    // dummy test to check if the test suite is working
    printf(YELLOW "Running dummy tests...\n" RESET);
    int test_dummy_count = sizeof(test_dummy) / sizeof(test_dummy[0]);
    test_count += test_dummy_count;
    for (int i = 0; i < test_dummy_count; i++)
        failed += test_dummy[i]();
    printf("\n");

    // cv tests
    printf(YELLOW "Running cv tests...\n" RESET);
    int test_cv_count = sizeof(test_cv) / sizeof(test_cv[0]);
    test_count += test_cv_count;
    for (int i = 0; i < test_cv_count; i++)
        failed += test_cv[i]();
    printf("\n");

    // matrix tests
    printf(YELLOW "Running matrix tests...\n" RESET);
    int test_matrix_count = sizeof(tests_matrix) / sizeof(tests_matrix[0]);
    test_count += test_matrix_count;
    for (int i = 0; i < test_matrix_count; i++)
        failed += tests_matrix[i]();
    printf("\n");

    // stop timer
    clock_t end = clock();
    float time_spent = (float)(end - begin) / CLOCKS_PER_SEC;

    printf("Ran %d tests in %f seconds. %d failed.\n", test_count, time_spent, failed);

    if (failed)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}
