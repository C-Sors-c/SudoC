#include "../include/libs_sdl.h"

/*
Take a surface in parameter and return a char array in 3D
with rows column and rgb for each dimension. RGB in 0-255 format.
*/
unsigned char ***sdl_surface_to_array(SDL_Surface *surface)
{
    int w = surface->w;
    int h = surface->h;
    Uint32* pixels = surface->pixels;
    unsigned char ***array = malloc(h * sizeof(unsigned char **));
    for (int i = 0; i < h; i++)
    {
        array[i] = malloc(w * sizeof(unsigned char *));
        for (int j = 0; j < w; j++)
        {
            array[i][j] = malloc(3 * sizeof(unsigned char));
            Uint32 pixel = pixels[i * w + j];
            SDL_GetRGB(pixel, surface->format, &array[i][j][0], &array[i][j][1], &array[i][j][2]);
        }
    }
    return array;
}