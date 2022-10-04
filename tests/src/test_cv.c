#include "../../sudoc/libs/include/utils.h"
#include "../../sudoc/libs/include/cv.h"
#include "../../sudoc/libs/include/libs_sdl.h"
#include "../include/test_cv.h"

int test_cv_rgb_to_gray()
{
    // load image
    SDL_Surface *surface = load_image("tests/samples/lena.png");
    unsigned char ***image_array = sdl_surface_to_array(surface);

    // init structures
    cv_image_rgb *img = cv_copy_image_rgb(image_array, surface->w, surface->h);

    // convert to gray
    cv_grayscale(img);

    // save image
    SDL_Surface *surface2 = array_to_sdl_surface(img->data, img->width, img->height);
    save_image(surface2, "tests/samples/lena_gray.png");

    // free memory
    SDL_FreeSurface(surface);
    SDL_FreeSurface(surface2);
    cv_free_image_rgb(img);
    free(image_array);

    return print_test(1, 1, "test_cv_rgb_to_gray");
}

int test_cv_gaussian_blur()
{
    // load image
    SDL_Surface *surface = load_image("tests/samples/lena.png");
    unsigned char ***image_array = sdl_surface_to_array(surface);

    // init structures
    cv_image_rgb *img = cv_copy_image_rgb(image_array, surface->w, surface->h);
    cv_image_gray *img_gray = cv_init_image_gray(img->width, img->height);
    cv_image_gray *img_blur = cv_init_image_gray(img->width, img->height);

    // convert to gray
    cv_rgb_to_gray(img, img_gray);

    // blur
    int k = 9;
    double sigma = (k - 1) / 6.0;

    cv_gaussian_blur(img_gray, img_blur, k, sigma);

    // convert to rgb
    cv_gray_to_rgb(img_blur, img);

    // save image
    SDL_Surface *surface2 = array_to_sdl_surface(img->data, img->width, img->height);
    save_image(surface2, "tests/samples/lena_gaussian_blur.png");

    // free memory
    SDL_FreeSurface(surface);
    SDL_FreeSurface(surface2);
    cv_free_image_rgb(img);
    free(image_array);

    return print_test(1, 1, "test_cv_gaussian_blur");
}