#include <stdio.h>
#include <stdlib.h>
#include "./include/utils.h"

#define ERROR_INFO printf(RED "ERROR at: " YELLOW " %s:%d:%s()\n" RESET, __FILE__, __LINE__, __func__)

int main(int argc, char *argv[])
{
    printf("Hello World!\n");
    ERROR_INFO;
    return EXIT_SUCCESS;
}
