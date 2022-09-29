#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* tempSurface = IMG_Load(path);
    if (tempSurface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(tempSurface, SDL_PIXELFORMAT_RGB888, 0);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(tempSurface);

    return surface;
}

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;
    return SDL_MapRGB(format, average, average, average);
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;

    SDL_PixelFormat* format = surface->format;

    int lock = SDL_LockSurface(surface);
    if (lock < 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    for(int i; i < len; i++)
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    SDL_UnlockSurface(surface); 


}

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // - Create a surface from the colored image.
    SDL_Surface* surface = load_image(argv[1]);

    // - Convert the surface into grayscale.
    surface_to_grayscale(surface);

    // - Free the surface.
    SDL_FreeSurface(surface);

    


    return EXIT_SUCCESS;
}
