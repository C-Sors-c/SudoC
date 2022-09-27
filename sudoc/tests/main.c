#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/test_add.h"

//lists of all tests to run
int (*tests[])() = {
    test_add_positive,
    test_add_negative,
};

int main()
{   
    // start timer
    clock_t begin = clock();

    //run all tests
    int test_count = sizeof(tests) / sizeof(tests[0]);
    int failed = 0;

    for (int i = 0; i < test_count; i++)
        failed += tests[i]();

    // stop timer
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Ran %d tests in %f seconds. %d failed.\n", test_count, time_spent, failed);
    
    if (failed)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}