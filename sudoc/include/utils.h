#pragma once

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

// Color codes for printing
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

int assert(int result, int expected, char name[]);
void init_rand();
bool cmp(float a, float b);
bool eq(float a, float b);
bool neq(float a, float b);
bool gt(float a, float b);
bool gte(float a, float b);
bool lt(float a, float b);
bool lte(float a, float b);
