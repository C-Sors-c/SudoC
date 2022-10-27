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
#define PI8 PI / 8.0
#define RGB 3
#define GRAYSCALE 1

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

#define NORM(x) MIN(MAX((x), 0), 1)

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

Matrix *CV_GET_SOBEL_KERNEL_X();
Matrix *CV_GET_SOBEL_KERNEL_Y();
Image *CV_SOBEL_PROCESS(Image *src, Image *dst, Image *dst_x, Image *dst_y);
Image *CV_SOBEL(Image *src, Image *dst);

float CV_ARCTAN2(Image *src, int c, int i, int j);
Image *CV_NON_MAX_SUPPRESSION(Image *src, Image *dst, Image *dst_x, Image *dst_y);
Image *CV_HYSTERESIS_THRESHOLDING(Image *src, Image *dst, float low, float high);
Image *CV_CANNY(Image *src, Image *dst, float low, float high);

float CV_THRESHOLD(Image *src);
Image *CV_OTSU(Image *src, Image *dst);
Image *CV_OR(Image *src1, Image *src2, Image *dst);
Image *CV_AND(Image *src1, Image *src2, Image *dst);
Image *CV_XOR(Image *src1, Image *src2, Image *dst);
Image *CV_NOT(Image *src, Image *dst);

Image *CV_DILATE(Image *src, Image *dst, int kernel_size);

Uint32 CV_RGB(Uint8 r, Uint8 g, Uint8 b);
Image *CV_DRAW_POINT(Image *dst, int x, int y, int width, Uint32 color);
Image *CV_DRAW_LINE(Image *dst, int x1, int y1, int x2, int y2, int width, Uint32 color);
Image *CV_DRAW_RECT(Image *dst, int x, int y, int w, int h, int width, Uint32 color);
Image *CV_DRAW_CIRCLE(Image *dst, int x, int y, int r, int width, Uint32 color);
Image *CV_DRAW_DIGIT(Image *dst, int x, int y, int digit, int size, Uint32 color);

int CV_FLOOR(float x);
int CV_ROUND(float x);
int CV_CEIL(float x);
int CV_COMPUTE_NUMANGLE(int min_theta, int max_theta, int theta_step);

int *CV_HOUGH_LINES(Image *src, int threshold, int *nlines);
int *CV_SIMPLIFY_HOUGH_LINES(int *lines, int nlines, int threshold, int *nsimplified);
Image *CV_DRAW_HOUGH_LINES(Image *dst, int *lines, int nlines, int weight, Uint32 color);
float CV_HOUGH_LINES_ORIENTATION(int *lines, int nlines);
int *CV_REMOVE_DIAGONALS(int *lines, int nlines, int *nsimplified);

Image *CV_ROTATE(Image *src, Image *dst, float angle, Uint32 background);
Image *CV_RESIZE(Image *src, Image *dst, float scale);
Image *CV_ZOOM(Image *src, Image *dst, float scale, Uint32 background);

Image *CV_GRID(Image *src, Image *dst);
