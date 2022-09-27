#include "../libs/include/add.h"
#include "../libs/include/utils.h"

int test_add()
{
    return printTest(add(15, 3), 18, "test_add");
}

int test_failure()
{
    return printTest(15, 18, "test_failure");
}
