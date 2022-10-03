#include "../include/cv.h"
#include <stdlib.h>
#include <err.h>
#include <math.h>

/// @brief Allocate memory for a cv_image_rgb structure.
/// @param image Pointer to the cv_image_rgb structure.
/// @param width Width of the image.
/// @param height Height of the image.
/// @return void
void cv_init_image_rgb(cv_image_rgb* image, int width, int height) {
    image->width = width;
    image->height = height;
    image->data = malloc(sizeof(char**) * height);

    if (image->data == NULL) {
        errx(EXIT_FAILURE, "Error: malloc failed in cv_init_image_rgb (1) (width=%d, height=%d)", width, height);
    }

    for (int i = 0; i < height; i++) {
        image->data[i] = malloc(sizeof(char*) * width);
        for (int j = 0; j < width; j++) {
            image->data[i][j] = malloc(sizeof(char) * 3);
        }
    }    
}

/// @brief Free memory allocated for a cv_image_rgb structure.
/// @param image Pointer to the cv_image_rgb structure.
/// @return void
void cv_free_image_rgb(cv_image_rgb* image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            free(image->data[i][j]);
        }
        free(image->data[i]);
    }
    free(image->data);
}

/// @brief Allocate memory for a cv_image_rgb_gray structure.
/// @param image Pointer to the cv_image_rgb_gray structure.
/// @param width Width of the image.
/// @param height Height of the image.
/// @return void
void cv_init_image_gray(cv_image_gray* image, int width, int height) {
    image->width = width;
    image->height = height;
    image->data = malloc(sizeof(char*) * height);

    if (image->data == NULL) {
        errx(EXIT_FAILURE, "Error: malloc failed in cv_init_image_gray (1) (width=%d, height=%d)", width, height);
    }

    for (int i = 0; i < height; i++) {
        image->data[i] = malloc(sizeof(char) * width);
    }    
}

/// @brief Free memory allocated for a cv_image_rgb_gray structure.
/// @param image Pointer to the cv_image_rgb_gray structure.
/// @return void
void cv_free_image_gray(cv_image_gray* image) {
    for (int i = 0; i < image->height; i++) {
        free(image->data[i]);
    }
    free(image->data);
}

/// @brief Allocate memory for a cv_image_rgb_normalized structure.
/// @param image Pointer to the cv_image_rgb_normalized structure.
/// @param width Width of the image.
/// @param height Height of the image.
/// @return void
void cv_init_image_rgb_normalized(cv_image_rgb_normalized* image, int width, int height) {
    image->width = width;
    image->height = height;
    image->data = malloc(sizeof(float**) * height);

    if (image->data == NULL) {
        errx(EXIT_FAILURE, "Error: malloc failed in cv_init_image_rgb_normalized (1) (width=%d, height=%d)", width, height);
    }

    for (int i = 0; i < height; i++) {
        image->data[i] = malloc(sizeof(float*) * width);
        for (int j = 0; j < width; j++) {
            image->data[i][j] = malloc(sizeof(float) * 3);
        }
    }    
}

/// @brief Free memory allocated for a cv_image_rgb_normalized structure.
/// @param image Pointer to the cv_image_rgb_normalized structure.
/// @return void
void cv_free_image_rgb_normalized(cv_image_rgb_normalized* image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            free(image->data[i][j]);
        }
        free(image->data[i]);
    }
    free(image->data);
}

/// @brief Allocate memory for a cv_image_gray_normalized structure.
/// @param image Pointer to the cv_image_gray_normalized structure.
/// @param width Width of the image.
/// @param height Height of the image.
/// @return void
void cv_init_image_gray_normalized(cv_image_gray_normalized* image, int width, int height) {
    image->width = width;
    image->height = height;
    image->data = malloc(sizeof(float*) * height);

    if (image->data == NULL) {
        errx(EXIT_FAILURE, "Error: malloc failed in cv_init_image_gray_normalized (1) (width=%d, height=%d)", width, height);
    }

    for (int i = 0; i < height; i++) {
        image->data[i] = malloc(sizeof(float) * width);
    }    
}

/// @brief Free memory allocated for a cv_image_gray_normalized structure.
/// @param image Pointer to the cv_image_gray_normalized structure.
/// @return void
void cv_free_image_gray_normalized(cv_image_gray_normalized* image) {
    for (int i = 0; i < image->height; i++) {
        free(image->data[i]);
    }
    free(image->data);
}

/// @brief Convert a cv_image_rgb structure to a cv_image_gray structure.
/// @param rgb Pointer to the cv_image_rgb structure.
/// @param gray Pointer to the cv_image_gray structure.
/// @return void
void cv_rgb_to_gray(cv_image_rgb* rgb, cv_image_gray* gray) {
    if (rgb->width != gray->width || rgb->height != gray->height) {
        errx(EXIT_FAILURE, "Error: cv_rgb_to_gray: image sizes do not match (rgb: %dx%d, gray: %dx%d)", rgb->width, rgb->height, gray->width, gray->height);
    }

    for (int i = 0; i < rgb->height; i++) {
        for (int j = 0; j < rgb->width; j++) {
            gray->data[i][j] = (rgb->data[i][j][0] + rgb->data[i][j][1] + rgb->data[i][j][2]) / 3;
        }
    }
}

/// @brief Convert a cv_image_gray structure to a cv_image_rgb structure.
/// @param gray Pointer to the cv_image_gray structure.
/// @param rgb Pointer to the cv_image_rgb structure.
/// @return void
void cv_gray_to_rgb(cv_image_gray* gray, cv_image_rgb* rgb) {
    if (rgb->width != gray->width || rgb->height != gray->height) {
        errx(EXIT_FAILURE, "Error: cv_gray_to_rgb: image sizes do not match (rgb: %dx%d, gray: %dx%d)", rgb->width, rgb->height, gray->width, gray->height);
    }

    for (int i = 0; i < rgb->height; i++) {
        for (int j = 0; j < rgb->width; j++) {
            rgb->data[i][j][0] = gray->data[i][j];
            rgb->data[i][j][1] = gray->data[i][j];
            rgb->data[i][j][2] = gray->data[i][j];
        }
    }
}

/// @brief Convert a cv_image_rgb structure to a cv_image_rgb_normalized structure.
/// @param rgb Pointer to the cv_image_rgb structure.
/// @param rgb_normalized Pointer to the cv_image_rgb_normalized structure.
/// @return void
void cv_normalize_image_rgb(cv_image_rgb* rgb, cv_image_rgb_normalized* rgb_normalized) {
    if (rgb->width != rgb_normalized->width || rgb->height != rgb_normalized->height) {
        errx(EXIT_FAILURE, "Error: cv_normalize_image_rgb: image sizes do not match (rgb: %dx%d, rgb_normalized: %dx%d)", rgb->width, rgb->height, rgb_normalized->width, rgb_normalized->height);
    }

    for (int i = 0; i < rgb->height; i++) {
        for (int j = 0; j < rgb->width; j++) {
            rgb_normalized->data[i][j][0] = rgb->data[i][j][0] / 255.0;
            rgb_normalized->data[i][j][1] = rgb->data[i][j][1] / 255.0;
            rgb_normalized->data[i][j][2] = rgb->data[i][j][2] / 255.0;
        }
    }
}

/// @brief Convert a cv_image_gray structure to a cv_image_gray_normalized structure.
/// @param gray Pointer to the cv_image_gray structure.
/// @param gray_normalized Pointer to the cv_image_gray_normalized structure.
/// @return void
void cv_normalize_image_gray(cv_image_gray* gray, cv_image_gray_normalized* gray_normalized) {
    if (gray->width != gray_normalized->width || gray->height != gray_normalized->height) {
        errx(EXIT_FAILURE, "Error: cv_normalize_image_gray: image sizes do not match (gray: %dx%d, gray_normalized: %dx%d)", gray->width, gray->height, gray_normalized->width, gray_normalized->height);
    }

    for (int i = 0; i < gray->height; i++) {
        for (int j = 0; j < gray->width; j++) {
            gray_normalized->data[i][j] = gray->data[i][j] / 255.0;
        }
    }
}

/// @brief Convert a cv_image_rgb_normalized structure to a cv_image_rgb structure.
/// @param rgb_normalized Pointer to the cv_image_rgb_normalized structure.
/// @param rgb Pointer to the cv_image_rgb structure.
/// @return void
void cv_denormalize_image_rgb(cv_image_rgb_normalized* rgb_normalized, cv_image_rgb* rgb) {
    if (rgb->width != rgb_normalized->width || rgb->height != rgb_normalized->height) {
        errx(EXIT_FAILURE, "Error: cv_denormalize_image_rgb: image sizes do not match (rgb: %dx%d, rgb_normalized: %dx%d)", rgb->width, rgb->height, rgb_normalized->width, rgb_normalized->height);
    }

    for (int i = 0; i < rgb->height; i++) {
        for (int j = 0; j < rgb->width; j++) {
            rgb->data[i][j][0] = rgb_normalized->data[i][j][0] * 255.0;
            rgb->data[i][j][1] = rgb_normalized->data[i][j][1] * 255.0;
            rgb->data[i][j][2] = rgb_normalized->data[i][j][2] * 255.0;
        }
    }
}

/// @brief Convert a cv_image_gray_normalized structure to a cv_image_gray structure.
/// @param gray_normalized Pointer to the cv_image_gray_normalized structure.
/// @param gray Pointer to the cv_image_gray structure.
/// @return void
void cv_denormalize_image_gray(cv_image_gray_normalized* gray_normalized, cv_image_gray* gray) {
    if (gray->width != gray_normalized->width || gray->height != gray_normalized->height) {
        errx(EXIT_FAILURE, "Error: cv_denormalize_image_gray: image sizes do not match (gray: %dx%d, gray_normalized: %dx%d)", gray->width, gray->height, gray_normalized->width, gray_normalized->height);
    }

    for (int i = 0; i < gray->height; i++) {
        for (int j = 0; j < gray->width; j++) {
            gray->data[i][j] = gray_normalized->data[i][j] * 255.0;
        }
    }
}


void cv_gaussian_blur(cv_image_gray* src, cv_image_gray* dst, int kernel_size, double sigma) {
    if (src->width != dst->width || src->height != dst->height) {
        errx(EXIT_FAILURE, "Error: cv_gaussian_blur: image sizes do not match (src: %dx%d, dst: %dx%d)", src->width, src->height, dst->width, dst->height);
    }
    
    if (kernel_size % 2 == 0) {
        errx(EXIT_FAILURE, "Error: cv_gaussian_blur: kernel_size must be odd");
    }

    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            double sum = 0.0;
            double weight_sum = 0.0;
            for (int k = -kernel_size / 2; k <= kernel_size / 2; k++) {
                for (int l = -kernel_size / 2; l <= kernel_size / 2; l++) {
                    int x = j + l;
                    int y = i + k;
                    if (x < 0) {
                        x = 0;
                    }
                    if (x >= src->width) {
                        x = src->width - 1;
                    }
                    if (y < 0) {
                        y = 0;
                    }
                    if (y >= src->height) {
                        y = src->height - 1;
                    }
                    double weight = exp(-(k * k + l * l) / (2 * sigma * sigma));
                    sum += src->data[y][x] * weight;
                    weight_sum += weight;
                }
            }
            dst->data[i][j] = sum / weight_sum;
        }
    }
}