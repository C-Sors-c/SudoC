#include "../include/test_add.h"


int test_add_positive()
{
    return print_test(add(15, 3), 18, "test_add_positive");
}

int test_add_negative()
{
    return print_test(add(-7, -3), -10, "test_add_negative");
}
