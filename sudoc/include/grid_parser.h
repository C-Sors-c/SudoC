#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku_utils.h"

void load_grid(char *filename, int res[][9]);

void save_grid(char *filename, int grid[][9]);
