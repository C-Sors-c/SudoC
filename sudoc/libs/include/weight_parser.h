#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

// TODO: implement a parser for the weight matrix
void load_weight(const char *filename, Matrix *weight, Matrix *bias);

void save_weight(const char *filename, Matrix *weight, Matrix *bias);
