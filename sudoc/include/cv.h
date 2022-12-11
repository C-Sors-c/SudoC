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
#define RGB 3
#define GRAYSCALE 1

#define FREE(ptr)       \
    do                  \
    {                   \
        if (ptr)        \
        {               \
            free(ptr);  \
            ptr = NULL; \
        }               \
    } while (0)

// chanel major access : data[height * width * channel + height * width + width]
#define PIXEL(image, c, i, j) \
    (image->data[(c) * (image)->h * (image)->w + (i) * (image)->w + (j)])

#define min(a, b) ((a) < (b) ? (a) : (b))

#define max(a, b) ((a) > (b) ? (a) : (b))

#define norm(x) min(max((x), 0), 1)

// print info about an error
#define DEBUG_INFO                                                            \
    printf("============================================================\n"); \
    printf(YELLOW "->  %s:%d:%s()\n" RESET, __FILE__, __LINE__, __func__)

#define ERRX(msg) errx(1, RED "%s" RESET, (msg))

// check if a pointer is NULL
#define ASSERT_PTR(ptr)           \
    do                            \
    {                             \
        if ((ptr) == NULL)        \
        {                         \
            DEBUG_INFO;           \
            ERRX("NULL pointer"); \
        }                         \
    } while (0)

#define ASSERT_IMG(image)            \
    do                               \
    {                                \
        if ((image) == NULL)         \
        {                            \
            DEBUG_INFO;              \
            ERRX("NULL image");      \
        }                            \
        if ((image)->data == NULL)   \
        {                            \
            DEBUG_INFO;              \
            ERRX("NULL image data"); \
        }                            \
    } while (0)

// check if the number of channels is valid
#define ASSERT_CHANNEL(image, n)                                     \
    do                                                               \
    {                                                                \
        if ((image)->c != (n))                                       \
        {                                                            \
            DEBUG_INFO;                                              \
            errx(1, "Invalid number of channels : %d (expected %d)", \
                 (image)->c, (n));                                   \
        }                                                            \
    } while (0)

#define ASSERT_DIM(image, c, height, width)                                     \
    do                                                                          \
    {                                                                           \
        ASSERT_IMG(image);                                                      \
        ASSERT_CHANNEL(image, c);                                               \
        if ((image)->h != (height))                                             \
        {                                                                       \
            DEBUG_INFO;                                                         \
            errx(1, "Invalid height : %d (expected %d)", (image)->h, (height)); \
        }                                                                       \
        if ((image)->w != (width))                                              \
        {                                                                       \
            DEBUG_INFO;                                                         \
            errx(1, "Invalid width : %d (expected %d)", (image)->w, (width));   \
        }                                                                       \
    } while (0)

#define ASSERT_MAT(matrix)                              \
    do                                                  \
    {                                                   \
        if ((matrix) == NULL)                           \
        {                                               \
            DEBUG_INFO;                                 \
            ERRX("NULL matrix");                        \
        }                                               \
        if ((matrix)->data == NULL)                     \
        {                                               \
            DEBUG_INFO;                                 \
            ERRX("NULL matrix data");                   \
        }                                               \
        if ((matrix)->dim1 <= 0 || (matrix)->dim2 <= 0) \
        {                                               \
            DEBUG_INFO;                                 \
            ERRX("Invalid matrix size");                \
        }                                               \
    } while (0)

// check if the index is valid
#define ASSERT_MAT4_INDEX(matrix, index)                                                             \
    do                                                                                               \
    {                                                                                                \
        if ((index) < 0 || (index) >= (matrix)->dim1)                                                \
        {                                                                                            \
            DEBUG_INFO;                                                                              \
            errx(1, RED "invalid index: got %d, expected [0, %d]\n" RESET, (index), (matrix)->dim1); \
        }                                                                                            \
    } while (0)

#define PRINT_IMG(image) \
    DEBUG_INFO;          \
    printf(BLUE "Image: c: %d, h: %d, w: %d\n" RESET, (image)->c, (image)->h, (image)->w)

void CV_FREE(Image **image);
Image *CV_INIT(int channels, int height, int width);
Image *CV_ZEROS(int channels, int height, int width);
Image *CV_ONES(int channels, int height, int width);
Image *CV_COPY(const Image *image);
void CV_COPY_TO(const Image *src, Image *dst);
Image *CV_COPY_REGION(const Image *image, int xstart, int ystart, int xend, int yend);

void CV_SHOW(const Image *image, char *title);
void CV_SHOW_MAT4(const Matrix4 *matrix, char *title, int index);

Image *CV_SURFACE_TO_IMG(SDL_Surface *surface);
SDL_Surface *CV_IMG_TO_SURFACE(const Image *image);
SDL_Surface *CV_LOAD_SURFACE(const char *path);

Matrix4 *CV_IMG_TO_MAT4(const Image *src, Matrix4 *dst, int index);
Image *CV_MAT4_TO_IMG(const Matrix4 *src, Image *dst, int index);

void CV_SAVE(const Image *image, char *path);
Image *CV_LOAD(const char *path, int mode);
char **CV_LIST_DIR(const char *path, int *count);

Image **CV_LOAD_FOLDER(const char *path, int *count, int mode);
Matrix4 *CV_LOAD_FOLDER_MAT4(const char *path, int *count, int mode);
Matrix4 *CV_LOAD_MAT4(const char *path, Matrix4 *matrix, int index, int batch_size, int mode);

Image *CV_RGB_TO_GRAY(const Image *src, Image *dst);
Image *CV_GRAY_TO_RGB(const Image *src, Image *dst);

Image *CV_APPLY_FILTER(const Image *src, Image *dst, const Matrix *kernel);
Matrix *CV_GET_GAUSSIAN_KERNEL(int size, float sigma);
Image *CV_GAUSSIAN_BLUR(const Image *src, Image *dst, int size, float sigma);
Image *CV_MEDIAN_BLUR(const Image *src, Image *dst, int size);
Image *CV_BILATERAL_FILTER(const Image *src, Image *dst, int size, float sigma_d, float sigma_r);

Matrix *CV_GET_SHARPEN_KERNEL(float sigma);
Image *CV_SHARPEN(const Image *src, Image *dst, float sigma);

Matrix *CV_GET_SOBEL_KERNEL_X();
Matrix *CV_GET_SOBEL_KERNEL_Y();
Image *CV_SOBEL_PROCESS(const Image *src, Image *dst, Image *dst_x, Image *dst_y);
Image *CV_SOBEL(const Image *src, Image *dst);

Image *CV_NON_MAX_SUPPRESSION(const Image *src, Image *dst, Image *dst_x, Image *dst_y);
Image *CV_THRESHOLD(const Image *src, Image *dst, float low, float high);
Image *CV_HYSTERISIS(const Image *src, Image *dst, float weak, float strong);
Image *CV_CANNY(const Image *src, Image *dst, float low, float high);

pixel_t CV_OTSU_THRESHOLD(const Image *src);
Image *CV_OTSU(const Image *src, Image *dst);
Image *CV_ADAPTIVE_THRESHOLD(const Image *src, Image *dst, int block_size, float otsu_weight, float c);
Image *CV_OR(const Image *src1, Image *src2, Image *dst);
Image *CV_AND(const Image *src1, Image *src2, Image *dst);
Image *CV_XOR(const Image *src1, Image *src2, Image *dst);
Image *CV_NOT(const Image *src, Image *dst);
Image *CV_SUB(const Image *src1, Image *src2, Image *dst);

Image *CV_DILATE(const Image *src, Image *dst, int k);
Image *CV_ERODE(const Image *src, Image *dst, int k);
Image *CV_OPEN(const Image *src, Image *dst, int k);
Image *CV_CLOSE(const Image *src, Image *dst, int k);

Uint32 CV_RGB(Uint8 r, Uint8 g, Uint8 b);
pixel_t CV_COLOR(Uint32 color, int channel);
Image *CV_DRAW_POINT(const Image *src, Image *dst, int x, int y, int width, Uint32 color);
Image *CV_DRAW_LINE(const Image *src, Image *dst, int x1, int y1, int x2, int y2, int width, Uint32 color);
Image *CV_DRAW_RECT(const Image *src, Image *dst, int x, int y, int w, int h, int width, Uint32 color);
Image *CV_DRAW_CIRCLE(const Image *src, Image *dst, int x, int y, int r, int width, Uint32 color);
Image *CV_DRAW_DIGIT(const Image *src, Image *dst, int x, int y, int digit, int size, Uint32 color);

int *CV_HOUGH_TRANSFORM(const Image *src, int threshold, int *nlines);
int *CV_MERGE_LINES(int *lines, int nlines, int threshold, int *nsimplified);
int *CV_HOUGH_LINES(const Image *src, int intersection_threshold, int merge_threshold, int *nlines);
Image *CV_DRAW_LINES(const Image *src, Image *dst, int *lines, int nlines, int weight, Uint32 color);
float CV_ORIENTATION(int *lines, int nlines);

int *CV_INTERSECTIONS_RAW(int *lines, int nlines, int *nintersection);
int *CV_INTERSECTIONS_SORT(int *intersections, int nintersections);
int *CV_INTERSECTIONS(int *lines, int nlines, int *nintersection);
int *CV_GRID_BOXES(int *intersections, int nintersections, int *nboxes);

int *CV_FIND_CONTOURS(const Image *src, int *nrects);
int *CV_JARVIS_MARCH(int *points, int npoints, int *nconvex);
int *CV_MIN_AREA_RECT(int *points, int n);
int *CV_MAX_RECTANGLE(const Image *src);

Image *CV_TRANSFORM(const Image *src, const Matrix *M, Tupple dsize, Tupple origin, Uint32 background);
Image *CV_ROTATE(const Image *src, float angle, bool resize, Uint32 background);
Image *CV_SCALE(const Image *src, float scale, Uint32 background);
Image *CV_RESIZE(const Image *src, Tupple dsize, Uint32 background);
Image *CV_ZOOM(const Image *src, float zoom, Uint32 background);
Image *CV_TRANSLATE(const Image *src, Tupple offset, Uint32 background);

Image *CV_RECONSTRUCT_IMAGE(Image *src, int grid[][9], int empty_cells[][9]);
