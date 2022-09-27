#include <stdlib.h>
#include "test_add.h"

// by default the test are successful
int ret = 0;

int main()
{
    ret = test_failure() || ret;
    ret = test_add() || ret;

    
    // if at least one test fails
    if (ret)
        return EXIT_FAILURE;
    // if every tests are successful
    else 
        return EXIT_SUCCESS;
}