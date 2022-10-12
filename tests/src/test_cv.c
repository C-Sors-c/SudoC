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

int test_cv_sharp()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *gray = cv_grayscale(img, NULL);
    Image *blur = cv_gaussian_blur(gray, NULL, 9, 1.5);
    Image *sharp = cv_sharp(blur, NULL, 3);

    cv_save_image(sharp, "tests/out/lena_sharp.png");

    cv_free_image(img);
    cv_free_image(gray);
    cv_free_image(blur);
    cv_free_image(sharp);

    return print_test(1, 1, "test_cv_sharp");
}

int test_cv_sobel()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *gray = cv_grayscale(img, NULL);
    Image *blur = cv_gaussian_blur(gray, NULL, 9, 3);
    Image *sobel = cv_sobel(blur, NULL, 3);

    cv_save_image(sobel, "tests/out/lena_sobel.png");

    cv_free_image(img);
    cv_free_image(gray);
    cv_free_image(blur);
    cv_free_image(sobel);

    return print_test(1, 1, "test_cv_sobel");
}

int test_cv_canny()
{
    Image *img = cv_image_from_path("tests/samples/sudoku.png");
    Image *gray = cv_grayscale(img, NULL);
    Image *canny = cv_canny(gray, NULL);

    cv_save_image(canny, "tests/out/sudoku_canny.png");

    cv_free_image(img);
    cv_free_image(gray);
    cv_free_image(canny);

    return print_test(1, 1, "test_cv_canny");
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