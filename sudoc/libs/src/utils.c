#include <stdio.h>
#include "../include/utils.h"

int print_test(int result, int expected, char name[])
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