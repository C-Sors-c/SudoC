#include "../../sudoc/include/utils.h"
#include "../../sudoc/include/cv.h"
#include "../../sudoc/include/libs_sdl.h"
#include "../include/test_cv.h"

int test_cv_rgb_to_gray()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *gray = cv_grayscale(img, NULL);

    cv_save_image(gray, "tests/out/lena_gray.png");
    
    cv_free_image(img);
    cv_free_image(gray);
    return print_test(1, 1, "test_cv_rgb_to_gray");
}

int test_cv_gaussian_blur()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *gray = cv_grayscale(img, NULL);
    Image *blur = cv_gaussian_blur(gray, NULL, 9, 1.5);

    cv_save_image(blur, "tests/out/lena_blur.png");

    cv_free_image(img);
    cv_free_image(gray);
    cv_free_image(blur);

    return print_test(1, 1, "test_cv_gaussian_blur");
}


int test_cv_rotate()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *rotated = cv_rotate(img, NULL, -39);

    cv_save_image(rotated, "tests/out/lena_rotated.png");
    cv_free_image(img);
    cv_free_image(rotated);
    return print_test(1, 1, "test_cv_gaussian_blur");
}