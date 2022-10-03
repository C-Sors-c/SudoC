#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/test_add.h"
#include "include/test_cv.h"
#include "../sudoc/libs/include/utils.h"

//lists of all tests to run
int (*test_dummy[])() = {
    test_add_positive,
    test_add_negative,
};

int (*test_cv[])() = {
    test_cv_init_image_rgb,
};

int main()
{   
    // start timer
    clock_t begin = clock();

    //run all tests
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

    // stop timer
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Ran %d tests in %f seconds. %d failed.\n", test_count, time_spent, failed);
    
    if (failed)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}