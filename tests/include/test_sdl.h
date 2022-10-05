
#pragma once

#include <sys/stat.h>
#include <stdbool.h>
#include "../../sudoc/include/utils.h"
#include "../../sudoc/include/libs_sdl.h"


SDL_Surface* array_to_sdl_surfaceGreen(int w, int h);
void create_and_save_red_image();
int test_sdl_surface_to_array_red();
int test_to_green();
