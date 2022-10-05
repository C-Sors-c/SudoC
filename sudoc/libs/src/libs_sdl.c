#include "../include/libs_sdl.h"
#include "../include/cv.h"

// load an image into a surface with SDL and return it without a window
SDL_Surface* load_image(char *path)
{
    SDL_Surface* tempImage = NULL;
    SDL_Surface* Image = NULL;
    tempImage = IMG_Load(path);
    if (tempImage != NULL)
    {
        //load image with the SDL_PIXELFORMAT_RGBA8888 format and no -Wint-conversion error
        Image = SDL_ConvertSurfaceFormat(tempImage, SDL_PIXELFORMAT_RGB888, 0);
        SDL_FreeSurface(tempImage);
    }
    return Image;
}

// save an image with png format from a surface with SDL
void save_image(SDL_Surface *surface, char *path)
{
    IMG_SavePNG(surface, path);
}

/*
    Take a surface in parameter and return a char array in 3D
    with rows column and rgb for each dimension. RGB in 0-255 format.
*/
unsigned char ***sdl_surface_to_array(SDL_Surface *surface)
{
    SDL_LockSurface(surface);
    int w = surface->w;
    int h = surface->h;
    //save the format of the surface
    SDL_PixelFormat *format = surface->format;
    Uint32* pixels = surface->pixels;
    unsigned char ***array = malloc(h * sizeof(unsigned char **));
    for (int i = 0; i < h; i++)
    {
        array[i] = malloc(w * sizeof(unsigned char *));
        for (int j = 0; j < w; j++)
        {
            array[i][j] = malloc(3 * sizeof(unsigned char));
            Uint32 pixel = pixels[i * w + j];
            SDL_GetRGB(pixel, format, &array[i][j][0], &array[i][j][1], &array[i][j][2]);
        }
    }
    SDL_UnlockSurface(surface);
    return array;
}

// transform a 3d unsigned char array to a surface
SDL_Surface* array_to_sdl_surface(unsigned char ***array, int w, int h)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);
    SDL_PixelFormat *format = surface->format;
    Uint32* pixels = surface->pixels;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            Uint32 pixel = SDL_MapRGB(format, array[i][j][0], array[i][j][1], array[i][j][2]);
            pixels[i * w + j] = pixel;
        }
    }
    SDL_UnlockSurface(surface);
    return surface;
}
