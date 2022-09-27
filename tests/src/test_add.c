#include "../../sudoc/libs/include/utils.h"
#include "../../sudoc/libs/include/add.h"

#include "../include/test_add.h"

// TODO: include logic to test more case per function

int test_add_positive()
{
    return print_test(add(15, 3), 18, "test_add_positive");
}

int test_add_negative()
{
    return print_test(add(-7, -3), -10, "test_add_negative");
}
