#include <stdio.h>
#include "../include/utils.h"

/// @brief Asserts that the given condition is true.
/// @param result The result of the condition.
/// @param expected The expected result of the condition.
/// @param name The name of the test.
/// @return 0 if the test passed, 1 if the test failed. (used for counting failed tests)
int assert(int result, int expected, char name[])
{
    if (result == expected)
    {
        printf(GREEN "%s: Success\n" RESET, name);
        return 0;
    }

    else
    {
        printf(RED "%s: Test failed, got %i, expected %i\n" RESET, name, result, expected);
        return 1;
    }
}