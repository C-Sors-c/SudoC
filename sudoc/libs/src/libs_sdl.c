#include <err.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

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