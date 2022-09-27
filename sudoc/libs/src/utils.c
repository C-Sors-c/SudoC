#include <stdio.h>
#include "../include/utils.h"

int printTest(int result, int expected, char name[])
{

    if (result == expected)
    {
        printf("%s: Success\n", name);
        return 0;
    }

    else
    {
        printf("%s: Test failed, got %i, expected %i\n", name, result, expected);
        return 1;
    }
}