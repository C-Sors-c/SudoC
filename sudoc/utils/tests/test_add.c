#include "../include/add.h"
#include <unity.h>

int test_add()
{
    TEST_ASSERT_EQUAL(5, add(2, 3));
    return 0;
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_add);
    return UNITY_END();
}