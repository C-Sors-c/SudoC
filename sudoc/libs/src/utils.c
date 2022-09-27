#include <stdio.h>
#include "../include/utils.h"

// Color codes for printing
#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

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