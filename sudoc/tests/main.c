#include <stdlib.h>
#include "include/test_add.h"

// by default the test are successful
int ret = 0;

int main()
{
    // add here all the tests
    ret = test_add_positive() || ret;
    ret = test_add_negative() || ret;
    
    // if at least one test fails
    if (ret)
        return EXIT_FAILURE;
    // if every tests are successful
    else 
        return EXIT_SUCCESS;
}