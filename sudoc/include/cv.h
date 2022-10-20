#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>

#include "../include/matrix.h"
#include "../include/utils.h"

typedef struct
{
    int width;
    int height;
    int channels;
    float ***data;
} Image;

void cv_free_image(Image *image);
Image *cv_image_init(int width, int height, int channels);
Image *cv_image_copy(Image *src);
Image *cv_image_from_u8(unsigned char ***src, int width, int height, int channels);
Image *cv_image_from_float(float ***src, int width, int height, int channels);
Image *cv_image_from_path(char *path);
Image *cv_image_from_surface(SDL_Surface *surface);
unsigned char ***cv_uint8_from_image(Image *src);
float ***cv_float_from_image(Image *src);
Matrix4 *cv_matrix_from_image(Image *src, Matrix4 *dst, int b);
SDL_Surface *cv_surface_from_path(char *path);
SDL_Surface *cv_surface_from_image(Image *image);
void cv_save_image(Image *image, char *path);
Image *cv_grayscale(Image *src, Image *dst);
void cv_apply_kernel(Image *src, Matrix *kernel);
Matrix *cv_compute_gaussian_kernel(int size, float sigma);
Image *cv_gaussian_blur(Image *src, Image *dst, int kernel_size, double sigma);
Image *cv_sharp(Image *src, Image *dst, int kernel_size);
Image *cv_sobel(Image *src, Image *dst);
Image *cv_canny(Image *src, Image *dst);
Image *cv_rotate(Image *src, Image *dst, float angle);
