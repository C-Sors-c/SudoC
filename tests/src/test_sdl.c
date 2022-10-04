#include "../include/test_sdl.h"

SDL_Surface *array_to_sdl_surfaceGreen(int w, int h)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            Uint32 pixel = SDL_MapRGB(format, 0, 255, 0);
            pixels[i * w + j] = pixel;
        }
    }
    SDL_UnlockSurface(surface);
    return surface;
}

void create_and_save_red_image()
{
    mkdir("tests/data", 0777);
    // create a 2x2 red image using array_to_sdl_surface from libs_sdl
    unsigned char ***array = malloc(2 * sizeof(unsigned char **));
    for (int i = 0; i < 2; i++)
    {
        array[i] = malloc(2 * sizeof(unsigned char *));
        for (int j = 0; j < 2; j++)
        {
            array[i][j] = malloc(3 * sizeof(unsigned char));
            array[i][j][0] = 255;
            array[i][j][1] = 0;
            array[i][j][2] = 0;
        }
    }
    SDL_Surface *surface = array_to_sdl_surface(array, 2, 2);
    save_image(surface, "tests/data/testRed.png");
    SDL_FreeSurface(surface);
    // free all the array
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

int test_sdl_surface_to_array_red()
{
    create_and_save_red_image();
    SDL_Surface *surface = load_image("tests/data/testRed.png");
    unsigned char ***array = sdl_surface_to_array(surface);

    unsigned char expected[2][2][3] = {
        {{255, 0, 0}, {255, 0, 0}},
        {{255, 0, 0}, {255, 0, 0}}};

    bool diff = true;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (array[i][j][k] != expected[i][j][k])
                {
                    diff = false;
                }
            }
        }
    }
    SDL_FreeSurface(surface);
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
    return print_test(diff, true, "test_sdl_surface_to_array_red");
}

int test_to_green()
{
    // load the red image then transform it to green and save it then reload it and compare it to the expected image

    SDL_Surface *surface = load_image("tests/data/testRed.png");
    SDL_Surface *surfaceGreen = array_to_sdl_surfaceGreen(surface->w, surface->h);
    save_image(surfaceGreen, "tests/data/testGreen.png");
    SDL_Surface *surfaceGreen2 = load_image("tests/data/testGreen.png");
    unsigned char ***arrayGreen = sdl_surface_to_array(surfaceGreen2);

    unsigned char expected[2][2][3] = {
        {{0, 255, 0}, {0, 255, 0}},
        {{0, 255, 0}, {0, 255, 0}}};

    bool diff = true;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (arrayGreen[i][j][k] != expected[i][j][k])
                {
                    diff = false;
                }
            }
        }
    }
    SDL_FreeSurface(surface);
    SDL_FreeSurface(surfaceGreen);
    SDL_FreeSurface(surfaceGreen2);
    // free all the array
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            free(arrayGreen[i][j]);
        }
        free(arrayGreen[i]);
    }
    free(arrayGreen);
    return print_test(diff, true, "test_to_green");
}