#pragma once
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

unsigned char ***sdl_surface_to_array(SDL_Surface *surface);
