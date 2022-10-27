#include "../../sudoc/include/utils.h"
#include "../../sudoc/include/cv.h"
#include "../include/test_cv.h"

int test_cv_load()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    CV_IMAGE_FREE(image);
    return assert(true, true, "test_cv_load");
}

int test_cv_save()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    CV_SAVE(image, "tests/out/test_cv_save.png");
    Image *image2 = CV_LOAD("tests/out/test_cv_save.png", RGB);

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
    Image **images = CV_LOAD_FOLDER("tests/samples", &n, RGB);

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
    Matrix4 *matrix = CV_MATRIX4_FROM_FOLDER("tests/samples/matrix", &n, RGB);

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

    Matrix4 *matrix = CV_MATRIX4_FROM_PATH(file, NULL, 0, 32, RGB);
    Image *image = CV_IMAGE_FROM_MATRIX4(matrix, NULL, 0);
    CV_SAVE(image, "tests/out/test_cv_load_image_with_matrix_0.png");

    for (int i = 1; i < n; i++)
    {
        file = files[i];
        matrix = CV_MATRIX4_FROM_PATH(file, matrix, i, 32, RGB);
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
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);
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
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *blurred = CV_GAUSSIAN_BLUR(image, NULL, 5, 0);
    CV_SAVE(blurred, "tests/out/test_cv_gaussian_blur.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blurred);
    return assert(true, true, "test_cv_gaussian_blur");
}

int test_cv_gaussian_blur_gray()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);
    Image *blurred = CV_GAUSSIAN_BLUR(image, NULL, 5, 0);
    CV_SAVE(blurred, "tests/out/test_cv_gaussian_blur_gray.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blurred);
    return assert(true, true, "test_cv_gaussian_blur_gray");
}

int test_cv_sharp()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *sharped = CV_SHARPEN(image, NULL, 1);

    CV_SAVE(sharped, "tests/out/test_cv_sharp.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(sharped);
    return assert(true, true, "test_cv_sharp");
}

int test_cv_sobel()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 5, 1);
    Image *sobel = CV_SOBEL(blur, NULL);

    CV_SAVE(sobel, "tests/out/test_cv_sobel.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(sobel);
    return assert(true, true, "test_cv_sobel");
}

int test_cv_canny()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 5, 0);
    Image *canny = CV_CANNY(blur, NULL, 0.05, 0.1);

    CV_SAVE(canny, "tests/out/test_cv_canny.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(canny);
    return assert(true, true, "test_cv_canny");
}

int test_cv_otsu()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", GRAYSCALE);

    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 3, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, 3);
    Image *otsu = CV_OTSU(sharp, NULL);

    CV_SAVE(otsu, "tests/out/test_cv_otsu.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(sharp);
    CV_IMAGE_FREE(otsu);
    return assert(true, true, "test_cv_otsu");
}

int test_cv_dilate()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 5, 0);
    Image *canny = CV_CANNY(blur, NULL, 0.05, 0.1);
    Image *dilation = CV_DILATE(canny, NULL, 3);

    CV_SAVE(dilation, "tests/out/test_cv_dilation.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(canny);
    CV_IMAGE_FREE(dilation);
    return assert(true, true, "test_cv_dilation");
}

int test_cv_hough_lines()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", RGB);
    Image *gray = CV_RGB_TO_GRAY(image, NULL);
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, 3, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, 3);
    Image *otsu = CV_OTSU(sharp, NULL);
    Image *canny = CV_CANNY(blur, NULL, 0.05, 0.15);
    Image * or = CV_OR(otsu, canny, NULL);
    // Image *dilation = CV_DILATE(otsu, NULL, 3);
    // Image *erosion = CV_ERODE(dilation, NULL, 5);

    int n = 0;
    int *lines = CV_HOUGH_LINES(or, 480, &n);

    CV_DRAW_HOUGH_LINES(image, lines, n, 1, CV_RGB(255, 0, 0));
    CV_SAVE(image, "tests/out/test_cv_hough_lines.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(gray);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(sharp);
    CV_IMAGE_FREE(otsu);
    CV_IMAGE_FREE(canny);
    CV_IMAGE_FREE(or);
    // CV_IMAGE_FREE(dilation);
    // CV_IMAGE_FREE(erosion);

    FREE(lines);

    return assert(true, true, "test_cv_hough_lines");
}

int test_cv_simplify_hough_lines()
{
    float angle = 0;

    Image *image = CV_LOAD("tests/samples/sudoku2.png", RGB);
    Image *z1 = CV_ZOOM(image, NULL, 0.6, 0xffffff);
    Image *r1 = CV_ROTATE(z1, NULL, angle, 0xffffff);

    Image *gray = CV_RGB_TO_GRAY(r1, NULL);
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, 3, 1);
    Image *canny = CV_CANNY(blur, NULL, 0.3, 0.5);

    // pass 1
    int n = 0;
    int n2 = 0;
    int *lines = CV_HOUGH_LINES(canny, 300, &n);
    int *simplified = CV_SIMPLIFY_HOUGH_LINES(lines, n, 30, &n2);
    float orientation = CV_HOUGH_LINES_ORIENTATION(simplified, n2);
    printf("orientation: %f\n", orientation);
    printf("n: %d\n", n);
    printf("n2: %d\n", n2);

    // pass 2
    Image *copy = CV_IMAGE_COPY(image);
    CV_ZOOM(copy, z1, 0.6, 0xffffff);
    CV_ROTATE(z1, r1, angle, 0xffffff);
    CV_ROTATE(r1, z1, -orientation, 0xffffff); // apply guessed orientation
    Image *dst = CV_ZOOM(z1, NULL, 1.666666666667, 0xffffff);

    CV_RGB_TO_GRAY(dst, gray);
    CV_GAUSSIAN_BLUR(gray, blur, 9, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, 5);
    Image *otsu = CV_OTSU(sharp, NULL);

    int n3 = 0;
    int n4 = 0;
    int n5 = 0;
    int *lines2 = CV_HOUGH_LINES(otsu, 300, &n3);
    int *simplified2 = CV_SIMPLIFY_HOUGH_LINES(lines2, n3, 33, &n4);
    int *clean_lines = CV_REMOVE_DIAGONALS(simplified2, n4, &n5);
    CV_DRAW_HOUGH_LINES(dst, clean_lines, n5, 2, CV_RGB(255, 0, 0));

    CV_SAVE(dst, "tests/out/test_cv_simplified_hough_lines.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(z1);
    CV_IMAGE_FREE(r1);
    CV_IMAGE_FREE(gray);
    CV_IMAGE_FREE(blur);
    CV_IMAGE_FREE(canny);
    CV_IMAGE_FREE(dst);

    FREE(lines);
    FREE(simplified);
    FREE(lines2);
    FREE(simplified2);

    return assert(true, true, "test_cv_simplify_hough_lines");
}

int test_cv_draw()
{
    Image *image = CV_ONES(3, 500, 500);

    CV_DRAW_LINE(image, 0, 0, 500, 500, 3, CV_RGB(255, 0, 0));
    CV_DRAW_LINE(image, 0, 500, 500, 0, 5, CV_RGB(255, 0, 255));
    CV_DRAW_RECT(image, 100, 100, 200, 200, 3, CV_RGB(0, 255, 0));
    CV_DRAW_CIRCLE(image, 250, 250, 100, 3, CV_RGB(0, 0, 255));
    CV_DRAW_DIGIT(image, 50, 50, 5, 30, CV_RGB(0, 0, 0));

    CV_SAVE(image, "tests/out/test_cv_draw.png");
    CV_IMAGE_FREE(image);

    return assert(true, true, "test_cv_draw");
}

int test_cv_rotate()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *rotated = CV_ROTATE(image, NULL, 45.0, 0x000000);

    CV_SAVE(rotated, "tests/out/test_cv_rotate.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(rotated);
    return assert(true, true, "test_cv_rotate");
}

int test_cv_resize()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *resized = CV_RESIZE(image, NULL, 0.5);

    CV_SAVE(resized, "tests/out/test_cv_resize.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(resized);
    return assert(true, true, "test_cv_resize");
}

int test_cv_zoom()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *zoomed = CV_ZOOM(image, NULL, 2.0, 0x000000);

    CV_SAVE(zoomed, "tests/out/test_cv_zoom.png");

    CV_IMAGE_FREE(image);
    CV_IMAGE_FREE(zoomed);
    return assert(true, true, "test_cv_zoom");
}
