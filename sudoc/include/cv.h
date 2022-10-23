#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>

#include "../include/matrix.h"
#include "../include/utils.h"

typedef float pixel_t;

typedef struct
{
    int c;
    int h;
    int w;
    pixel_t *data;
} Image;

#define PI 3.14159265358979323846
#define CV_RGB 3
#define CV_GRAYSCALE 1

#define FREE(ptr)     \
    do                \
    {                 \
        free((ptr));  \
        (ptr) = NULL; \
    } while (0)

// chanel major access : data[height * width * channel + height * width + width]
#define PIXEL(image, c, i, j) \
    (image->data[(c) * (image)->h * (image)->w + (i) * (image)->w + (j)])

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// print info about an error
#define ERROR_INFO                                                            \
    printf("============================================================\n"); \
    printf(YELLOW "->  %s:%d:%s()\n" RESET, __FILE__, __LINE__, __func__)

// check if a pointer is NULL
#define CV_CHECK_PTR(ptr)                  \
    if ((ptr) == NULL)                     \
    {                                      \
        ERROR_INFO;                        \
        errx(1, RED "NULL pointer" RESET); \
    }

#define CV_CHECK_IMAGE(image)                     \
    if ((image) == NULL || (image)->data == NULL) \
    {                                             \
        ERROR_INFO;                               \
        errx(1, RED "NULL image" RESET);          \
    }

// check if the pixel is in the image
#define CV_CHECK_PIXEL(image, c, i, j) \
    ((c) >= 0 && (c) < (image)->c && (i) >= 0 && (i) < (image)->h && (j) >= 0 && (j) < (image)->w)

// check if the number of channels is valid
#define CV_CHECK_CHANNEL(image, n)                                                     \
    if ((image)->c != (n))                                                             \
    {                                                                                  \
        ERROR_INFO;                                                                    \
        errx(1, RED "invalid number of channels: %d != %d \n" RESET, (image)->c, (n)); \
    }

// check if the number of channels is valid
#define CV_CMP_DIMS(m, n)                                                         \
    if ((m) != (n))                                                               \
    {                                                                             \
        ERROR_INFO;                                                               \
        errx(1, RED "invalid number of dimensions: %d != %d \n" RESET, (m), (n)); \
    }

// check if the index is valid
#define CV_CHECK_INDEX(matrix, index)                                                                \
    if ((index) < 0 || (index) >= (matrix)->dim1)                                                    \
    {                                                                                                \
        ERROR_INFO;                                                                                  \
        errx(1, RED "invalid index: %d < 0 || %d >= %d \n" RESET, (index), (index), (matrix)->dim1); \
    }

#define CV_PRINT_IMAGE(image) \
    ERROR_INFO;               \
    printf(BLUE "Image: c: %d, h: %d, w: %d\n" RESET, (image)->c, (image)->h, (image)->w)

void CV_IMAGE_FREE(Image *image);
Image *CV_IMAGE_INIT(int channels, int height, int width);
Image *CV_ZEROS(int channels, int height, int width);
Image *CV_ONES(int channels, int height, int width);
Image *CV_IMAGE_COPY(Image *image);

void CV_IMAGE_SHOW(Image *image, char *title);
void CV_IMAGE_SHOW_MATRIX4(Matrix4 *matrix, char *title, int index);

Image *CV_IMAGE_FROM_SURFACE(SDL_Surface *surface);
SDL_Surface *CV_SURFACE_FROM_IMAGE(Image *image);
SDL_Surface *CV_SURFACE_FROM_PATH(char *path);

Matrix4 *CV_MATRIX4_FROM_IMAGE(Image *image, Matrix4 *matrix, int index);
Image *CV_IMAGE_FROM_MATRIX4(Matrix4 *matrix, Image *image, int index);

void CV_SAVE(Image *image, char *path);
Image *CV_LOAD(char *path, int mode);
char **CV_LIST_FILES(char *path, int *count);

Image **CV_LOAD_FOLDER(char *path, int *count, int mode);
Matrix4 *CV_MATRIX4_FROM_FOLDER(char *path, int *count, int mode);
Matrix4 *CV_MATRIX4_FROM_PATH(char *path, Matrix4 *matrix, int index, int batch_size, int mode);

Image *CV_RGB_TO_GRAY(Image *src, Image *dst);
Image *CV_GRAY_TO_RGB(Image *src, Image *dst);

Image *CV_APPLY_FILTER(Image *src, Image *dst, Matrix *kernel);

Matrix *CV_GET_GAUSSIAN_KERNEL(int size, float sigma);
Image *CV_GAUSSIAN_BLUR(Image *src, Image *dst, int size, float sigma);

Matrix *CV_GET_SHARPEN_KERNEL(float sigma);
Image *CV_SHARPEN(Image *src, Image *dst, float sigma);