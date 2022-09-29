#include <err.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

float*** surface_to_3d(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixel;
    int width = surface->w;
    int height = surface->h;
    SDL_PixelFormat* format = surface->format;
    Uint8 r, g, b;
    
    float array_image[height][width][3];

    for (int row = 0; row < width; row++)
    {
        for (int col = 0; col < height; col++)
        {
            SDL_GetRGB(pixels[row][col], format, &r, &g, &b);
            array_image[row][col][0] = ((float) r) / 255.0;
            array_image[row][col][1] = (float) g / 255.0;
            array_image[row][col][2] = (float) b / 255.0;
            }
        
    }
    
    
}