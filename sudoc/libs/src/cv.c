#include "../include/cv.h"

#include <err.h>
#include <math.h>
#include <stdlib.h>

#pragma region Image

void cv_free_image(Image *image) {
    if (image == NULL)
        return;

    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            free(image->data[i][j]);
        }
        free(image->data[i]);
    }
    free(image->data);
    free(image);
}

Image *cv_image_init(int width, int height, int channels) {
    Image *image = malloc(sizeof(Image));
    if (image == NULL) {
        err(1, "malloc");
    }

    image->width = width;
    image->height = height;
    image->channels = channels;
    image->data = malloc(sizeof(float **) * height);

    if (image->data == NULL) {
        errx(1, "malloc");
    }

    for (int i = 0; i < height; i++) {
        image->data[i] = malloc(sizeof(float *) * width);

        if (image->data[i] == NULL) {
            errx(1, "malloc");
        }

        for (int j = 0; j < width; j++) {
            image->data[i][j] = calloc(channels, sizeof(float));

            if (image->data[i][j] == NULL) {
                errx(1, "malloc");
            }
        }
    }
    return image;
}

Image *cv_image_copy(Image *src) {
    if (src == NULL)
        errx(1, "source image is null");

    Image *img = cv_image_init(src->width, src->height, src->channels);
    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            for (int k = 0; k < src->channels; k++) {
                img->data[i][j][k] = src->data[i][j][k];
            }
        }
    }
    return img;
}

Image *cv_image_from_u8(unsigned char ***src, int width, int height, int channels) {
    if (src == NULL)
        errx(1, "source image is null");

    Image *img = cv_image_init(width, height, channels);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < channels; k++) {
                img->data[i][j][k] = (float)src[i][j][k];
            }
        }
    }
    return img;
}

Image *cv_image_from_float(float ***src, int width, int height, int channels) {
    if (src == NULL)
        errx(1, "source image is null");

    Image *img = cv_image_init(width, height, channels);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < channels; k++) {
                img->data[i][j][k] = src[i][j][k];
            }
        }
    }
    return img;
}

Image *cv_image_from_path(char *path) {
    SDL_Surface *surface = cv_surface_from_path(path);
    Image *image = cv_image_from_surface(surface);
    SDL_FreeSurface(surface);
    return image;
}

Image *cv_image_from_surface(SDL_Surface *surface) {
    SDL_LockSurface(surface);

    int w = surface->w;
    int h = surface->h;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    Image *image = cv_image_init(w, h, 3);

    if (image != NULL)
        printf("image is not null\n");

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Uint32 pixel = pixels[i * w + j];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, format, &r, &g, &b);
            image->data[i][j][0] = (float)r;
            image->data[i][j][1] = (float)g;
            image->data[i][j][2] = (float)b;
        }
    }

    SDL_UnlockSurface(surface);
    return image;
}

unsigned char ***cv_uint8_from_image(Image *src) {
    if (src == NULL)
        errx(1, "source image is null");

    unsigned char ***img = malloc(sizeof(unsigned char **) * src->height);

    if (img == NULL)
        errx(1, "malloc");

    for (int i = 0; i < src->height; i++) {
        img[i] = malloc(sizeof(unsigned char *) * src->width);

        if (img[i] == NULL)
            errx(1, "malloc");

        for (int j = 0; j < src->width; j++) {
            img[i][j] = malloc(sizeof(unsigned char) * 3);

            if (img[i][j] == NULL)
                errx(1, "malloc");

            if (src->channels == 1) {
                for (int k = 0; k < 3; k++)
                    img[i][j][k] = (unsigned char)src->data[i][j][0];
            } else if (src->channels == 3) {
                for (int k = 0; k < 3; k++)
                    img[i][j][k] = (unsigned char)src->data[i][j][k];
            } else
                errx(1, "invalid number of channels");
        }
    }
    return img;
}

float ***cv_float_from_image(Image *src) {
    if (src == NULL)
        errx(1, "source image is null");

    float ***img = malloc(sizeof(float **) * src->height);

    if (img == NULL)
        errx(1, "malloc");

    for (int i = 0; i < src->height; i++) {
        img[i] = malloc(sizeof(float *) * src->width);

        if (img[i] == NULL)
            errx(1, "malloc");

        for (int j = 0; j < src->width; j++) {
            img[i][j] = malloc(sizeof(float) * src->channels);

            if (img[i][j] == NULL)
                errx(1, "malloc");

            for (int k = 0; k < src->channels; k++)
                img[i][j][k] = src->data[i][j][k];
        }
    }
    return img;
}

Matrix *cv_matrix_from_image(Image *src) {
    if (src == NULL)
        errx(1, RED "source image is null" RESET);

    if (src->channels != 1)
        errx(1, RED "invalid number of channels, must be 1" RESET);

    Matrix *mat = matrix_init(src->height, src->width, NULL);
    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            for (int k = 0; k < src->channels; k++) {
                mat->data[i * src->width + j][k] = src->data[i][j][k];
            }
        }
    }
    return mat;
}

SDL_Surface *cv_surface_from_path(char *path) {
    SDL_Surface *tempImage = NULL;
    SDL_Surface *Image = NULL;
    tempImage = IMG_Load(path);
    if (tempImage == NULL) {
        errx(1, "Unable to load image %s: %s", path, IMG_GetError());
    }
    Image = SDL_ConvertSurfaceFormat(tempImage, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tempImage);
    return Image;
}

SDL_Surface *cv_surface_from_image(Image *image) {
    SDL_Surface *surface = SDL_CreateRGBSurface(0, image->width, image->height, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            if (image->channels == 1) {
                Uint8 r = (Uint8)image->data[i][j][0];
                Uint8 g = (Uint8)image->data[i][j][0];
                Uint8 b = (Uint8)image->data[i][j][0];

                Uint32 pixel = SDL_MapRGB(format, r, g, b);
                pixels[i * image->width + j] = pixel;
            } else if (image->channels == 3) {
                Uint8 r = (Uint8)image->data[i][j][0];
                Uint8 g = (Uint8)image->data[i][j][1];
                Uint8 b = (Uint8)image->data[i][j][2];

                Uint32 pixel = SDL_MapRGB(format, r, g, b);
                pixels[i * image->width + j] = pixel;
            } else
                errx(1, "invalid number of channels");
        }
    }

    SDL_UnlockSurface(surface);
    return surface;
}

void cv_save_image(Image *image, char *path) {
    SDL_Surface *surface = cv_surface_from_image(image);
    IMG_SavePNG(surface, path);
    SDL_FreeSurface(surface);
}

#pragma endregion Image

Image *cv_grayscale(Image *src, Image *dst) {
    if (src == NULL) {
        errx(EXIT_FAILURE, "Error: cv_grayscale: src is NULL");
    }

    if (dst == NULL) {
        dst = cv_image_init(src->width, src->height, 1);
    }

    if (src->width != dst->width || src->height != dst->height) {
        errx(
            EXIT_FAILURE,
            "Error: cv_grayscale: image sizes do not match (src: %dx%d, dst: %dx%d)",
            src->width, src->height, dst->width, dst->height);
    }

    if (src->channels == 1) {
        return dst;
    }

    if (src->channels != 3) {
        errx(EXIT_FAILURE, "Error: cv_grayscale: src must have 3 channels");
    }

    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            double sum = src->data[i][j][0] + src->data[i][j][1] + src->data[i][j][2];
            dst->data[i][j][0] = sum / 3.0;
        }
    }

    return dst;
}

void cv_apply_kernel(Image *src, Matrix *kernel) {
    if (src->channels != 1 && src->channels != 3) {
        errx(1, "invalid number of channels");
    }

    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            float sum = 0.0;
            for (int k = -kernel->dim1 / 2; k <= kernel->dim1 / 2; k++) {
                for (int l = -kernel->dim2 / 2; l <= kernel->dim2 / 2; l++) {
                    int x = j + l;
                    int y = i + k;

                    // 0 padding
                    if (x < 0 || x >= src->width || y < 0 || y >= src->height)
                        continue;

                    int kx = l + kernel->dim2 / 2;
                    int ky = k + kernel->dim1 / 2;

                    sum += src->data[y][x][0] * kernel->data[ky][kx];
                    if (src->channels == 3) {
                        sum += src->data[y][x][1] * kernel->data[ky][kx];
                        sum += src->data[y][x][2] * kernel->data[ky][kx];
                    }
                }
            }
            src->data[i][j][0] = sum;
            if (src->channels == 3) {
                src->data[i][j][1] = sum;
                src->data[i][j][2] = sum;
            }
        }
    }
}

Matrix *cv_compute_gaussian_kernel(int size, float sigma) {
    Matrix *kernel = matrix_init(size, size, NULL);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float x = j - size / 2.0;
            float y = i - size / 2.0;

            float value = exp(-(x * x + y * y) / (2.0 * sigma * sigma));
            kernel->data[i][j] = value / (2.0 * M_PI * sigma * sigma);
        }
    }

    return kernel;
}

Image *cv_gaussian_blur(Image *src, Image *dst, int kernel_size, double sigma) {
    if (src == NULL) {
        errx(EXIT_FAILURE, "Error: cv_gaussian_blur: src is NULL");
    }

    if (dst == NULL) {
        dst = cv_image_copy(src);
    }

    if (src->width != dst->width || src->height != dst->height) {
        errx(EXIT_FAILURE,
             "Error: cv_gaussian_blur: image sizes do not match (src: %dx%d, dst: %dx%d)",
             src->width, src->height, dst->width, dst->height);
    }

    if (kernel_size % 2 == 0) {
        errx(EXIT_FAILURE, "Error: cv_gaussian_blur: kernel_size must be odd");
    }

    if (sigma <= 0) {
        sigma = (kernel_size - 1) / 6.0;
    }

    Matrix *kernel = cv_compute_gaussian_kernel(kernel_size, sigma);
    cv_apply_kernel(dst, kernel);
    matrix_destroy(kernel);
    return dst;
}