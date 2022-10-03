#pragma once
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

SDL_Surface* load_image(char *path);
void save_image(SDL_Surface *surface, char *path);
unsigned char ***sdl_surface_to_array(SDL_Surface *surface);
SDL_Surface* array_to_sdl_surface(unsigned char ***array, int w, int h);