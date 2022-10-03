#include "../../sudoc/libs/include/utils.h"
#include "../../sudoc/libs/include/cv.h"
#include "../include/test_cv.h"

int test_cv_init_image_rgb()
{
    cv_image_rgb image;
    cv_init_image_rgb(&image, 10, 10);
    int res = print_test(image.width, 10, "test_cv_init_image_rgb");

    cv_free_image_rgb(&image);
    return res;
}

int test_cv_init_image_gray()
{
    cv_image_gray image;
    cv_init_image_gray(&image, 10, 10);
    int res = print_test(image.width, 10, "test_cv_init_image_gray");

    cv_free_image_gray(&image);
    return res;
}

int test_cv_init_image_rgb_normalized()
{
    cv_image_rgb_normalized image;
    cv_init_image_rgb_normalized(&image, 10, 10);
    int res = print_test(image.width, 10, "test_cv_init_image_rgb_normalized");

    cv_free_image_rgb_normalized(&image);
    return res;
}

int test_cv_init_image_gray_normalized()
{
    cv_image_gray_normalized image;
    cv_init_image_gray_normalized(&image, 10, 10);
    int res = print_test(image.width, 10, "test_cv_init_image_gray_normalized");

    cv_free_image_gray_normalized(&image);
    return res;
}

int test_cv_rgb_to_gray()
{
    cv_image_rgb rgb;
    cv_init_image_rgb(&rgb, 10, 10);
    cv_image_gray gray;
    cv_init_image_gray(&gray, 10, 10);

    cv_rgb_to_gray(&rgb, &gray);

    int res = print_test(gray.width, 10, "test_cv_rgb_to_gray");

    cv_free_image_rgb(&rgb);
    cv_free_image_gray(&gray);
    return res;
}

int test_cv_gray_to_rgb()
{
    cv_image_gray gray;
    cv_init_image_gray(&gray, 10, 10);
    cv_image_rgb rgb;
    cv_init_image_rgb(&rgb, 10, 10);

    cv_gray_to_rgb(&gray, &rgb);

    int res = print_test(rgb.width, 10, "test_cv_gray_to_rgb");

    cv_free_image_gray(&gray);
    cv_free_image_rgb(&rgb);
    return res;
}

int test_cv_normalize_image_rgb()
{
    cv_image_rgb rgb;
    cv_init_image_rgb(&rgb, 10, 10);
    cv_image_rgb_normalized rgb_normalized;
    cv_init_image_rgb_normalized(&rgb_normalized, 10, 10);

    cv_normalize_image_rgb(&rgb, &rgb_normalized);

    int res = print_test(rgb_normalized.width, 10, "test_cv_normalize_image_rgb");

    cv_free_image_rgb(&rgb);
    cv_free_image_rgb_normalized(&rgb_normalized);
    return res;
}

int test_cv_normalize_image_gray()
{
    cv_image_gray gray;
    cv_init_image_gray(&gray, 10, 10);
    cv_image_gray_normalized gray_normalized;
    cv_init_image_gray_normalized(&gray_normalized, 10, 10);

    cv_normalize_image_gray(&gray, &gray_normalized);

    int res = print_test(gray_normalized.width, 10, "test_cv_normalize_image_gray");

    cv_free_image_gray(&gray);
    cv_free_image_gray_normalized(&gray_normalized);
    return res;
}
