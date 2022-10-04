#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/test_add.h"
#include "include/test_matrix.h"
#include "include/test_sdl.h"

// lists of all tests to run
int (*tests[])() = {
    test_add_positive,
    test_add_negative,
    test_matrix_add,
    test_matrix_subtract,
    test_matrix_multiply,
    test_matrix_multiply_scalar,
    test_matrix_transpose,
    test_matrix4_add,
    test_matrix4_subtract,
    test_matrix4_multiply,
    test_matrix4_multiply_scalar,
    test_sdl_surface_to_array_red,
    test_to_green,
};

int main()
{
    // start timer
    clock_t begin = clock();

    // run all tests
    int test_count = sizeof(tests) / sizeof(tests[0]);
    int failed = 0;

    for (int i = 0; i < test_count; i++)
        failed += tests[i]();

    // stop timer
    clock_t end = clock();
    float time_spent = (float)(end - begin) / CLOCKS_PER_SEC;

    printf("Ran %d tests in %f seconds. %d failed.\n", test_count, time_spent, failed);

    if (failed)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}
