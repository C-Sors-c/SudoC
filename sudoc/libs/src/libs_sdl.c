#include "../include/libs_sdl.h"

/*
Take a surface in parameter and return a char array in 3D
with rows column and rgb for each dimension. RGB in 0-255 format.
*/
char ***surface_to_3d(SDL_Surface *surface)
{
    Uint32 *pixels = surface->pixels;
    int cols = surface->w;
    int rows = surface->h;
    SDL_PixelFormat *format = surface->format;
    Uint8 r, g, b;

    char array_image[rows][cols][3];

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            SDL_GetRGB(pixels[i * cols + j], format, &r, &g, &b);
            array_image[i][j][0] = r;
            array_image[i][j][1] = g;
            array_image[i][j][2] = b;
        }
    }
    return array_image;
}