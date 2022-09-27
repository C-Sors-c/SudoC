#include "../../libs/include/utils.h"
#include "../../libs/include/add.h"

#include "../include/test_add.h"

int test_add_positive()
{
    return printTest(add(15, 3), 18, "test_add_positive");
}

int test_add_negative()
{
    return printTest(add(-7, -3), -10, "test_add_negative");
}
