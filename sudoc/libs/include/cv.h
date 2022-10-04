#pragma once

typedef struct
{
    int width;
    int height;
    unsigned char ***data;
} cv_image_rgb;

typedef struct
{
    int width;
    int height;
    float ***data;
} cv_image_rgb_normalized;

typedef struct
{
    int width;
    int height;
    unsigned char **data;
} cv_image_gray;

typedef struct
{
    int width;
    int height;
    float **data;
} cv_image_gray_normalized;

cv_image_rgb *cv_init_image_rgb(int width, int height);
cv_image_rgb *cv_copy_image_rgb(unsigned char ***image, int width, int height);
void cv_free_image_rgb(cv_image_rgb *image);

cv_image_gray *cv_init_image_gray(int width, int height);
cv_image_gray *cv_copy_image_gray(unsigned char **image, int width, int height);
void cv_free_image_gray(cv_image_gray *image);

cv_image_rgb_normalized *cv_init_image_rgb_normalized(int width, int height);
cv_image_rgb_normalized *cv_copy_image_rgb_normalized(float ***image, int width, int height);
void cv_free_image_rgb_normalized(cv_image_rgb_normalized *image);

cv_image_gray_normalized *cv_init_image_gray_normalized(int width, int height);
cv_image_gray_normalized *cv_copy_image_gray_normalized(float **image, int width, int height);
void cv_free_image_gray_normalized(cv_image_gray_normalized *image);

void cv_rgb_to_gray(cv_image_rgb *rgb, cv_image_gray *gray);
void cv_gray_to_rgb(cv_image_gray *gray, cv_image_rgb *rgb);

void cv_normalize_image_rgb(cv_image_rgb *rgb, cv_image_rgb_normalized *rgb_normalized);
void cv_normalize_image_gray(cv_image_gray *gray, cv_image_gray_normalized *gray_normalized);
void cv_denormalize_image_rgb(cv_image_rgb_normalized *rgb_normalized, cv_image_rgb *rgb);
void cv_denormalize_image_gray(cv_image_gray_normalized *gray_normalized, cv_image_gray *gray);

void cv_grayscale(cv_image_rgb *rgb);
void cv_gaussian_blur(cv_image_gray *src, cv_image_gray *dst, int kernel_size, double sigma);