#include "../../sudoc/include/utils.h"
#include "../../sudoc/include/cv.h"
#include "../include/test_cv.h"

int test_cv_load()
{
    Image *image = CV_LOAD("tests/samples/lena.png", CV_RGB);
    CV_IMAGE_FREE(image);
    return assert(true, true, "test_cv_load");
}

int test_cv_save()
{
    Image *image = CV_LOAD("tests/samples/lena.png", CV_RGB);
    CV_SAVE(image, "tests/out/test_cv_save.png");
    Image *image2 = CV_LOAD("tests/out/test_cv_save.png", CV_RGB);

    for (int i = 0; i < image->w * image->h * image->c; i++)
        if (image->data[i] != image2->data[i])
        {
            CV_IMAGE_FREE(image);
            CV_IMAGE_FREE(image2);
            return assert(false, true, "test_cv_save");
        }

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(image2);
    return assert(true, true, "test_cv_save");
}

int test_cv_zeros()
{
    Image *image = CV_ZEROS(3, 100, 100);
    for (int i = 0; i < image->w * image->h * image->c; i++)
        if (image->data[i] != 0)
        {
            CV_IMAGE_FREE(image);
            return assert(false, true, "test_cv_zeros");
        }

    CV_SAVE(image, "tests/out/test_cv_zeros.png");
    CV_IMAGE_FREE(image);
    return assert(true, true, "test_cv_zeros");
}

int test_cv_ones()
{
    Image *image = CV_ONES(3, 100, 100);
    for (int i = 0; i < image->w * image->h * image->c; i++)
        if (image->data[i] != 1.0)
        {
            int a = image->data[i];
            CV_IMAGE_FREE(image);
            return assert(a, true, "test_cv_ones");
        }

    CV_SAVE(image, "tests/out/test_cv_ones.png");
    CV_IMAGE_FREE(image);
    return assert(true, true, "test_cv_ones");
}

int test_cv_load_folder()
{
    int n;
    Image **images = CV_LOAD_FOLDER("tests/samples", &n, CV_RGB);

    for (int i = 0; i < n; i++)
    {
        char path[100];
        sprintf(path, "tests/out/test_load_images_%d.png", i);
        CV_SAVE(images[i], path);
    }

    for (int i = 0; i < n; i++)
        CV_IMAGE_FREE(images[i]);
    FREE(images);

    return assert(true, true, "test_cv_load_folder");
}

int test_cv_matrix_from_folder()
{
    int n;
    Matrix4 *matrix = CV_MATRIX4_FROM_FOLDER("tests/samples/matrix", &n, CV_RGB);

    if (n != 3)
    {
        matrix4_destroy(matrix);
        return assert(n, 3, "test_cv_matrix_from_folder");
    }

    Image *image = CV_IMAGE_FROM_MATRIX4(matrix, NULL, 0);

    for (int i = 1; i < n; i++)
    {
        char path[100];
        sprintf(path, "tests/out/test_cv_load_images_with_matrix_%d.png", i);
        CV_IMAGE_FROM_MATRIX4(matrix, image, i);
        CV_SAVE(image, path);
    }

    CV_IMAGE_FREE(image);
    matrix4_destroy(matrix);
    return assert(true, true, "test_cv_matrix_from_folder");
}

int test_cv_matrix_from_path()
{
    int n;
    char **files = CV_LIST_FILES("tests/samples/matrix", &n);

    CV_CHECK_PTR(files);
    if (n != 3)
        return assert(n, 3, "test_cv_matrix_from_path");

    char *file = files[0];

    Matrix4 *matrix = CV_MATRIX4_FROM_PATH(file, NULL, 0, 32, CV_RGB);
    Image *image = CV_IMAGE_FROM_MATRIX4(matrix, NULL, 0);
    CV_SAVE(image, "tests/out/test_cv_load_image_with_matrix_0.png");

    for (int i = 1; i < n; i++)
    {
        file = files[i];
        matrix = CV_MATRIX4_FROM_PATH(file, matrix, i, 32, CV_RGB);
        image = CV_IMAGE_FROM_MATRIX4(matrix, image, i);

        char path[100];
        sprintf(path, "tests/out/test_cv_load_image_with_matrix_%d.png", i);
        CV_SAVE(image, path);
    }

    matrix4_destroy(matrix);
    CV_IMAGE_FREE(image);
    for (int i = 0; i < n; i++)
        FREE(files[i]);
    FREE(files);

    return assert(true, true, "test_cv_matrix_from_path");
}

int test_cv_list_files()
{
    int n;
    char **files = CV_LIST_FILES("tests/samples/matrix", &n);

    for (int i = 0; i < n; i++)
        FREE(files[i]);
    FREE(files);

    // we expect to have 3 files in the folder
    return assert(n, 3, "test_cv_list_files");
}

int test_cv_rgb_to_gray()
{
    Image *image = CV_LOAD("tests/samples/lena.png", CV_GRAYSCALE);
    CV_SAVE(image, "tests/out/test_cv_rgb_to_gray.png");

    CV_IMAGE_FREE(image);
    return assert(true, true, "test_cv_rgb_to_gray");
}

int test_cv_get_gaussian_kernel()
{
    Matrix *kernel = CV_GET_GAUSSIAN_KERNEL(5, 1.0);
    float expected[25] = {
        0.0029690167, 0.01330621, 0.02193823, 0.01330621, 0.0029690167,
        0.01330621, 0.059634295, 0.09832033, 0.059634295, 0.01330621,
        0.02193823, 0.09832033, 0.16210282, 0.09832033, 0.02193823,
        0.01330621, 0.059634295, 0.09832033, 0.059634295, 0.01330621,
        0.0029690167, 0.01330621, 0.02193823, 0.01330621, 0.0029690167};

    for (int i = 0; i < 25; i++)
        if (kernel->data[i] != expected[i])
        {
            float x = kernel->data[i];
            matrix_destroy(kernel);
            return assert(x, expected[i], "test_cv_get_gaussian_kernel");
        }

    matrix_destroy(kernel);
    return assert(true, true, "test_cv_get_gaussian_kernel");
}

int test_cv_gaussian_blur()
{
    Image *image = CV_LOAD("tests/samples/lena.png", CV_RGB);
    Image *blurred = CV_GAUSSIAN_BLUR(image, NULL, 9, 2.0);
    CV_SAVE(blurred, "tests/out/test_cv_gaussian_blur.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blurred);
    return assert(true, true, "test_cv_gaussian_blur");
}

int test_cv_gaussian_blur_gray()
{
    Image *image = CV_LOAD("tests/samples/lena.png", CV_GRAYSCALE);
    Image *blurred = CV_GAUSSIAN_BLUR(image, NULL, 9, 2.0);
    CV_SAVE(blurred, "tests/out/test_cv_gaussian_blur_gray.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blurred);
    return assert(true, true, "test_cv_gaussian_blur_gray");
}

int test_cv_sharp()
{
    Image *image = CV_LOAD("tests/samples/lena.png", CV_RGB);
    Image *sharped = CV_SHARPEN(image, NULL, 1);

    CV_SAVE(sharped, "tests/out/test_cv_sharp.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(sharped);
    return assert(true, true, "test_cv_sharp");
}

int test_cv_sobel()
{
    Image *image = CV_LOAD("tests/samples/sudoku2.png", CV_GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 9, 2.0);
    Image *sharp = CV_SHARPEN(blur, NULL, 1);
    Image *sobel = CV_SOBEL(sharp, NULL);

    CV_SAVE(sobel, "tests/out/test_cv_sobel.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(sharp);
    CV_IMAGE_FREE(sobel);
    return assert(true, true, "test_cv_sobel");
}

int test_cv_canny()
{
    Image *image = CV_LOAD("tests/samples/sudoku2.png", CV_GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 9, 2.0);
    Image *sharp = CV_SHARPEN(blur, NULL, 1);
    Image *sobel = CV_SOBEL(sharp, NULL);
    Image *canny = CV_CANNY(sobel, NULL, 0.1, 0.2);

    CV_SAVE(canny, "tests/out/test_cv_canny.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(sharp);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(sobel);
    CV_IMAGE_FREE(canny);
    return assert(true, true, "test_cv_canny");
}

int test_cv_otsu()
{
    Image *image = CV_LOAD("tests/samples/sudoku2.png", CV_GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 9, 1.0);
    Image *sharp = CV_SHARPEN(blur, NULL, 2);
    Image *otsu = CV_OTSU(sharp, NULL);

    CV_SAVE(otsu, "tests/out/test_cv_otsu.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(sharp);
    CV_IMAGE_FREE(otsu);
    return assert(true, true, "test_cv_otsu");
}

/*


int test_cv_gaussian_blur()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *gray = cv_grayscale(img, NULL);
    Image *blur = cv_gaussian_blur(gray, NULL, 9, 1.5);

    cv_save_image(blur, "tests/out/lena_blur.png");

    cv_free_image(img);
    cv_free_image(gray);
    cv_free_image(blur);

    return test(1, 1, "test_cv_gaussian_blur");
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

    return test(1, 1, "test_cv_sharp");
}

int test_cv_sobel()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *gray = cv_grayscale(img, NULL);
    Image *blur = cv_gaussian_blur(gray, NULL, 9, 3);
    Image *sobel = cv_sobel(blur, NULL);

    cv_save_image(sobel, "tests/out/lena_sobel.png");

    cv_free_image(img);
    cv_free_image(gray);
    cv_free_image(blur);
    cv_free_image(sobel);

    return test(1, 1, "test_cv_sobel");
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

    return test(1, 1, "test_cv_canny");
}

int test_cv_rotate()
{
    Image *img = cv_image_from_path("tests/samples/lena.png");
    Image *rotated = cv_rotate(img, NULL, -39);

    cv_save_image(rotated, "tests/out/lena_rotated.png");
    cv_free_image(img);
    cv_free_image(rotated);
    return test(1, 1, "test_cv_gaussian_blur");
}

*/