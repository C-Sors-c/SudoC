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

void init_rand()
{
    srand(time(NULL));
}

bool cmp(float a, float b)
{
    return fabs(a - b) < 0.0001;
}

bool eq(float a, float b)
{
    return cmp(a, b);
}

bool neq(float a, float b)
{
    return !eq(a, b);
}

bool gt(float a, float b)
{
    return a > b;
}

bool gte(float a, float b)
{
    return a > b || eq(a, b);
}

bool lt(float a, float b)
{
    return a < b;
}

bool lte(float a, float b)
{
    return a < b || eq(a, b);
}