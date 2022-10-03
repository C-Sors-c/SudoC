#pragma once

typedef struct cv_image_rgb cv_image_rgb;
typedef struct cv_image_gray cv_image_gray;
typedef struct cv_image_rgb_normalized cv_image_rgb_normalized;
typedef struct cv_image_gray_normalized cv_image_gray_normalized;

void cv_init_image_rgb(cv_image_rgb* image, int width, int height);
void cv_free_image_rgb(cv_image_rgb* image);
void cv_init_image_gray(cv_image_gray* image, int width, int height);
void cv_free_image_gray(cv_image_gray* image);
void cv_init_image_rgb_normalized(cv_image_rgb_normalized* image, int width, int height);
void cv_free_image_rgb_normalized(cv_image_rgb_normalized* image);
void cv_init_image_gray_normalized(cv_image_gray_normalized* image, int width, int height);
void cv_free_image_gray_normalized(cv_image_gray_normalized* image);
void cv_rgb_to_gray(cv_image_rgb* rgb, cv_image_gray* gray);
void cv_gray_to_rgb(cv_image_gray* gray, cv_image_rgb* rgb);
void cv_normalize_image_rgb(cv_image_rgb* rgb, cv_image_rgb_normalized* rgb_normalized);
void cv_normalize_image_gray(cv_image_gray* gray, cv_image_gray_normalized* gray_normalized);
void cv_denormalize_image_rgb(cv_image_rgb_normalized* rgb_normalized, cv_image_rgb* rgb);
void cv_denormalize_image_gray(cv_image_gray_normalized* gray_normalized, cv_image_gray* gray);