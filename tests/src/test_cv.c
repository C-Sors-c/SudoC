#include "../../sudoc/include/utils.h"
#include "../../sudoc/include/cv.h"
#include "../include/test_cv.h"

int test_cv_load()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    CV_FREE(&image);
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
            CV_FREE(&image);
            CV_FREE(&image2);
            return assert(false, true, "test_cv_save");
        }

    CV_FREE(&image);
    CV_FREE(&image2);
    return assert(true, true, "test_cv_save");
}

int test_cv_zeros()
{
    Image *image = CV_ZEROS(3, 100, 100);
    for (int i = 0; i < image->w * image->h * image->c; i++)
        if (image->data[i] != 0)
        {
            CV_FREE(&image);
            return assert(false, true, "test_cv_zeros");
        }

    CV_SAVE(image, "tests/out/test_cv_zeros.png");
    CV_FREE(&image);
    return assert(true, true, "test_cv_zeros");
}

int test_cv_ones()
{
    Image *image = CV_ONES(3, 100, 100);
    for (int i = 0; i < image->w * image->h * image->c; i++)
        if (image->data[i] != 1.0)
        {
            int a = image->data[i];
            CV_FREE(&image);
            return assert(a, true, "test_cv_ones");
        }

    CV_SAVE(image, "tests/out/test_cv_ones.png");
    CV_FREE(&image);
    return assert(true, true, "test_cv_ones");
}

int test_cv_load_folder()
{
    int n;
    Image **images = CV_LOAD_FOLDER("tests/samples", &n, RGB);

    for (int i = 0; i < n; i++)
    {
        char path[100];
        snprintf(path, 100, "tests/out/test_load_images_%d.png", i);
        CV_SAVE(images[i], path);
    }

    for (int i = 0; i < n; i++)
        CV_FREE(&images[i]);
    FREE(images);

    return assert(true, true, "test_cv_load_folder");
}

int test_cv_matrix_from_folder()
{
    int n;
    Matrix4 *matrix = CV_LOAD_FOLDER_MAT4("tests/samples/matrix", &n, RGB);

    if (n != 3)
    {
        matrix4_destroy(matrix);
        return assert(n, 3, "test_cv_matrix_from_folder");
    }

    Image *image = CV_MAT4_TO_IMG(matrix, NULL, 0);

    for (int i = 1; i < n; i++)
    {
        char path[100];
        snprintf(path, 100, "tests/out/test_load_images_with_matrix_%d.png", i);
        CV_MAT4_TO_IMG(matrix, image, i);
        CV_SAVE(image, path);
    }

    CV_FREE(&image);
    matrix4_destroy(matrix);
    return assert(true, true, "test_cv_matrix_from_folder");
}

int test_cv_matrix_from_path()
{
    int n;
    char **files = CV_LIST_DIR("tests/samples/matrix", &n);

    ASSERT_PTR(files);
    if (n != 3)
        return assert(n, 3, "test_cv_matrix_from_path");

    char *file = files[0];

    Matrix4 *matrix = CV_LOAD_MAT4(file, NULL, 0, 32, RGB);
    Image *image = CV_MAT4_TO_IMG(matrix, NULL, 0);
    CV_SAVE(image, "tests/out/test_cv_load_image_with_matrix_0.png");

    for (int i = 1; i < n; i++)
    {
        file = files[i];
        matrix = CV_LOAD_MAT4(file, matrix, i, 32, RGB);
        image = CV_MAT4_TO_IMG(matrix, image, i);

        char path[100];
        snprintf(path, 100, "tests/out/test_cv_load_image_with_matrix_%d.png", i);
        CV_SAVE(image, path);
    }

    matrix4_destroy(matrix);
    CV_FREE(&image);
    for (int i = 0; i < n; i++)
        FREE(files[i]);
    FREE(files);

    return assert(true, true, "test_cv_matrix_from_path");
}

int test_cv_list_files()
{
    int n;
    char **files = CV_LIST_DIR("tests/samples/matrix", &n);

    for (int i = 0; i < n; i++)
        FREE(files[i]);
    FREE(files);

    // we expect to have 3 files in the folder
    return assert(n, 3, "test_cv_list_files");
}

int test_cv_rgb_to_gray()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);

    CV_RGB_TO_GRAY(image, image);

    CV_SAVE(image, "tests/out/test_cv_rgb_to_gray.png");

    CV_FREE(&image);

    return assert(true, true, "test_cv_rgb_to_gray");
}

int test_cv_gray_to_rgb()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);

    CV_GRAY_TO_RGB(image, image);

    CV_SAVE(image, "tests/out/test_cv_gray_to_rgb.png");

    CV_FREE(&image);

    return assert(true, true, "test_cv_gray_to_rgb");
}

int test_cv_get_gaussian_kernel()
{
    Matrix *kernel = CV_GET_GAUSSIAN_KERNEL(5, 1);
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

    CV_GAUSSIAN_BLUR(image, image, 5, 1);
    CV_SAVE(image, "tests/out/test_cv_gaussian_blur.png");

    CV_FREE(&image);
    return assert(true, true, "test_cv_gaussian_blur");
}

int test_cv_gaussian_blur_gray()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);

    CV_GAUSSIAN_BLUR(image, image, 5, 1);
    CV_SAVE(image, "tests/out/test_cv_gaussian_blur_gray.png");

    CV_FREE(&image);
    return assert(true, true, "test_cv_gaussian_blur_gray");
}

int test_cv_sharp()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *sharped = CV_SHARPEN(image, NULL, 1);

    CV_SAVE(sharped, "tests/out/test_cv_sharp.png");

    CV_FREE(&image);
    CV_FREE(&sharped);
    return assert(true, true, "test_cv_sharp");
}

int test_cv_sobel()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);

    CV_GAUSSIAN_BLUR(image, image, 3, 1);
    CV_SOBEL(image, image);

    CV_SAVE(image, "tests/out/test_cv_sobel.png");

    CV_FREE(&image);

    return assert(true, true, "test_cv_sobel");
}

int test_cv_canny()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);

    CV_GAUSSIAN_BLUR(image, image, 5, 1);
    CV_CANNY(image, image, 0.05, 0.15);

    CV_SAVE(image, "tests/out/test_cv_canny.png");

    CV_FREE(&image);
    return assert(true, true, "test_cv_canny");
}

int test_cv_otsu()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", GRAYSCALE);

    CV_OTSU(image, image);

    CV_SAVE(image, "tests/out/test_cv_otsu.png");

    CV_FREE(&image);
    return assert(true, true, "test_cv_otsu");
}

int test_cv_adaptive_threshold()
{
    char *img1 = "tests/samples/sudoku1.jpeg";
    char *img2 = "tests/samples/sudoku2.png";
    char *img3 = "tests/samples/sudoku.png";
    char *img4 = "tests/samples/sudoku1.png";

    Image *image = CV_LOAD(img1, GRAYSCALE);
    CV_GAUSSIAN_BLUR(image, image, 5, 1);
    CV_SHARPEN(image, image, 10);
    CV_ADAPTIVE_THRESHOLD(image, image, 5, 0.5, 0.5);

    CV_NOT(image, image);
    CV_DILATE(image, image, 3);
    CV_ERODE(image, image, 3);

    CV_SAVE(image, "tests/out/test_cv_adaptive_threshold.png");

    CV_FREE(&image);

    return assert(true, true, "test_cv_adaptive_threshold");
}

int test_cv_dilate()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);

    CV_OTSU(image, image);
    CV_NOT(image, image);
    CV_DILATE(image, image, 3);

    CV_SAVE(image, "tests/out/test_cv_dilate.png");

    CV_FREE(&image);
    return assert(true, true, "test_cv_dilate");
}

int test_cv_erode()
{
    Image *image = CV_LOAD("tests/samples/lena.png", GRAYSCALE);

    CV_OTSU(image, image);
    CV_NOT(image, image);
    CV_ERODE(image, image, 3);

    CV_SAVE(image, "tests/out/test_cv_erode.png");

    CV_FREE(&image);
    return assert(true, true, "test_cv_erode");
}

int test_cv_hough_lines()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.png", RGB);
    Image *processed = CV_COPY(image);

    CV_RGB_TO_GRAY(processed, processed);
    CV_GAUSSIAN_BLUR(processed, processed, 5, 1);
    CV_SHARPEN(processed, processed, 10);
    CV_ADAPTIVE_THRESHOLD(processed, processed, 5, 0.5, 0.5);
    CV_NOT(processed, processed);
    CV_DILATE(processed, processed, 3);
    CV_ERODE(processed, processed, 3);

    CV_SOBEL(processed, processed);

    int n = 0;
    int *lines = CV_HOUGH_TRANSFORM(processed, 300, &n);

    CV_DRAW_LINES(image, image, lines, n, 1, CV_RGB(255, 0, 0));

    CV_SAVE(image, "tests/out/test_cv_hough_lines.png");

    CV_FREE(&image);
    CV_FREE(&processed);

    FREE(lines);

    return assert(true, true, "test_cv_hough_lines");
}

int test_cv_hough_lines_merged()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", RGB);
    Image *processed = CV_COPY(image);

    CV_RGB_TO_GRAY(processed, processed);
    CV_GAUSSIAN_BLUR(processed, processed, 5, 1);

    CV_SHARPEN(processed, processed, 10);
    CV_ADAPTIVE_THRESHOLD(processed, processed, 5, 0.5, 0.5);

    CV_NOT(processed, processed);

    CV_DILATE(processed, processed, 3);

    CV_ERODE(processed, processed, 3);
    CV_SOBEL(processed, processed);

    int n = 0;
    int *lines = CV_HOUGH_LINES(processed, 300, 35, &n);

    CV_DRAW_LINES(image, image, lines, n, 2, CV_RGB(255, 0, 0));
    CV_SAVE(image, "tests/out/test_cv_test_cv_hough_lines_merged.png");

    CV_FREE(&image);
    CV_FREE(&processed);
    FREE(lines);

    return assert(true, true, "test_cv_hough_lines_merged");
}

int test_cv_intersections()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", RGB);
    Image *processed = CV_COPY(image);

    CV_RGB_TO_GRAY(processed, processed);
    CV_GAUSSIAN_BLUR(processed, processed, 5, 1);
    CV_SHARPEN(processed, processed, 10);
    CV_ADAPTIVE_THRESHOLD(processed, processed, 5, 0.5, 0.5);
    CV_NOT(processed, processed);
    CV_DILATE(processed, processed, 3);
    CV_ERODE(processed, processed, 3);
    CV_SOBEL(processed, processed);

    int n = 0;
    int *lines = CV_HOUGH_LINES(processed, 300, 35, &n);

    int ni = 0;
    int *intersections = CV_INTERSECTIONS(lines, n, &ni);

    if (ni < 100)
        return assert(ni, 100, "test_cv_intersections");

    for (int i = 0; i < ni; i++)
    {
        int x = intersections[i * 2];
        int y = intersections[i * 2 + 1];
        CV_DRAW_POINT(image, image, x, y, 5, CV_RGB(255, 0, 0));
    }

    CV_SAVE(image, "tests/out/test_cv_intersections.png");

    FREE(lines);
    FREE(intersections);

    CV_FREE(&image);
    CV_FREE(&processed);

    return assert(true, true, "test_cv_intersections");
}

int test_cv_boxes()
{
    Image *image = CV_LOAD("tests/samples/sudoku.png", RGB);
    Image *processed = CV_COPY(image);

    CV_RGB_TO_GRAY(processed, processed);
    CV_GAUSSIAN_BLUR(processed, processed, 5, 1);
    CV_SHARPEN(processed, processed, 10);
    CV_ADAPTIVE_THRESHOLD(processed, processed, 5, 0.5, 0.5);
    CV_NOT(processed, processed);
    CV_DILATE(processed, processed, 3);
    CV_ERODE(processed, processed, 3);
    CV_SOBEL(processed, processed);

    int n = 0;
    int *lines = CV_HOUGH_LINES(processed, 300, 35, &n);

    int ni = 0;
    int *intersections = CV_INTERSECTIONS(lines, n, &ni);

    if (ni < 100)
        return assert(ni, 100, "test_cv_boxes");

    int nb = 0;
    int *boxes = CV_GRID_BOXES(intersections, ni, &nb);

    int padding = 3;
    for (int i = 0; i < nb; i++)
    {
        int x1 = boxes[i * 4 + 0] + padding * 2;
        int y1 = boxes[i * 4 + 1] + padding * 2;
        int x2 = boxes[i * 4 + 2] - padding;
        int y2 = boxes[i * 4 + 3] - padding;

        int rw = x2 - x1;
        int rh = y2 - y1;

        CV_DRAW_RECT(image, image, x1, y1, rw, rh, 1, CV_RGB(255, 0, 0));
    }

    CV_SAVE(image, "tests/out/test_cv_boxes.png");

    CV_FREE(&image);
    CV_FREE(&processed);

    FREE(lines);
    FREE(intersections);
    FREE(boxes);

    return assert(true, true, "test_cv_boxes");
}

int test_cv_save_boxes()
{
    Image *image = CV_LOAD("tests/samples/sudoku.png", RGB);
    Image *processed = CV_COPY(image);

    CV_RGB_TO_GRAY(processed, processed);
    CV_GAUSSIAN_BLUR(processed, processed, 5, 1);
    CV_SHARPEN(processed, processed, 10);
    CV_ADAPTIVE_THRESHOLD(processed, processed, 5, 0.5, 0.5);
    CV_NOT(processed, processed);
    CV_DILATE(processed, processed, 3);
    CV_ERODE(processed, processed, 3);
    CV_SOBEL(processed, processed);

    int n = 0;
    int *lines = CV_HOUGH_LINES(processed, 300, 35, &n);

    int ni = 0;
    int *intersections = CV_INTERSECTIONS(lines, n, &ni);

    if (ni < 100)
        return assert(ni, 100, "test_cv_boxes");

    int nb = 0;
    int *boxes = CV_GRID_BOXES(intersections, ni, &nb);

    Image *box;
    int padding = 3;
    for (int i = 0; i < nb; i++)
    {
        int x1 = boxes[i * 4 + 0] + padding * 2;
        int y1 = boxes[i * 4 + 1] + padding * 2;
        int x2 = boxes[i * 4 + 2] - padding;
        int y2 = boxes[i * 4 + 3] - padding;
        box = CV_COPY_REGION(image, x1, y1, x2, y2);

        char path[100];
        snprintf(path, 100, "tests/out/box/test_cv_save_boxes_%d.png", i);

        CV_SAVE(box, path);
        CV_FREE(&box);
    }

    for (int i = 0; i < nb; i++)
    {
        int x1 = boxes[i * 4 + 0] + padding * 2;
        int y1 = boxes[i * 4 + 1] + padding * 2;
        int x2 = boxes[i * 4 + 2] - padding;
        int y2 = boxes[i * 4 + 3] - padding;

        int rw = x2 - x1;
        int rh = y2 - y1;

        CV_DRAW_RECT(image, image, x1, y1, rw, rh, 1, CV_RGB(255, 0, 0));
    }

    CV_SAVE(image, "tests/out/test_cv_save_boxes.png");

    CV_FREE(&image);
    CV_FREE(&processed);

    FREE(lines);
    FREE(intersections);
    FREE(boxes);

    return assert(true, true, "test_cv_boxes");
}

int test_cv_find_countours()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", RGB);
    Image *processed = CV_COPY(image);

    int k = 5;

    CV_RGB_TO_GRAY(processed, processed);
    CV_GAUSSIAN_BLUR(processed, processed, k, 1);
    CV_SHARPEN(processed, processed, k * 2);
    CV_ADAPTIVE_THRESHOLD(processed, processed, k, 0.5, 0.5);
    CV_NOT(processed, processed);
    CV_DILATE(processed, processed, 3);
    CV_ERODE(processed, processed, 3);

    CV_SOBEL(processed, processed);

    int n = 0;
    int *lines = CV_HOUGH_LINES(processed, 300, 35, &n);

    int nr = 0;
    int *pts = CV_FIND_LARGEST_CONTOUR(processed, &nr);

    for (int i = 0; i < nr; i++)
    {
        int x = pts[i * 2 + 0];
        int y = pts[i * 2 + 1];

        CV_DRAW_POINT(image, image, x, y, 3, CV_RGB(255, 0, 0));
    }

    CV_SAVE(image, "tests/out/test_cv_find_countours.png");

    CV_FREE(&image);
    CV_FREE(&processed);

    FREE(lines);
    FREE(pts);

    return assert(true, true, "test_cv_find_countours");
}

int test_cv_draw()
{
    Image *image = CV_ONES(3, 500, 500);

    CV_DRAW_LINE(image, image, 0, 0, 500, 500, 3, CV_RGB(255, 0, 0));
    CV_DRAW_LINE(image, image, 0, 500, 500, 0, 5, CV_RGB(255, 0, 255));
    CV_DRAW_RECT(image, image, 100, 100, 200, 200, 3, CV_RGB(0, 255, 0));
    CV_DRAW_CIRCLE(image, image, 250, 250, 100, 3, CV_RGB(0, 0, 255));
    CV_DRAW_DIGIT(image, image, 50, 50, 5, 30, CV_RGB(0, 0, 0));

    CV_SAVE(image, "tests/out/test_cv_draw.png");
    CV_FREE(&image);

    return assert(true, true, "test_cv_draw");
}

int test_cv_transform()
{
    Image *image = CV_LOAD("tests/samples/warp.png", RGB);

    Tupple pA = {50, 0};
    Tupple pB = {150, 0};
    Tupple pC = {200, 200};
    Tupple pD = {0, 200};

    Tupple pE = {0, 0};
    Tupple pF = {200, 0};
    Tupple pG = {200, 200};
    Tupple pH = {0, 200};

    Tupple *src = malloc(sizeof(Tupple) * 4);
    Tupple *dst = malloc(sizeof(Tupple) * 4);

    src[0] = pA;
    src[1] = pB;
    src[2] = pC;
    src[3] = pD;

    dst[0] = pE;
    dst[1] = pF;
    dst[2] = pG;
    dst[3] = pH;

    Matrix *M = matrix_transformation(src, dst);
    Image * tf = CV_TRANSFORM(image, M, T(image->w, image->h), T(0, 0), CV_RGB(0, 0, 0));

    CV_SAVE(tf, "tests/out/test_cv_transform.png");

    CV_FREE(&image);
    CV_FREE(&tf);
    FREE(src);
    FREE(dst);
    matrix_destroy(M);

    return assert(true, true, "test_cv_transform");
}

int test_cv_rotate()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *rotated = CV_ROTATE(image, 45, true, CV_RGB(0, 0, 0));

    CV_SAVE(rotated, "tests/out/test_cv_rotate.png");

    CV_FREE(&image);
    CV_FREE(&rotated);
    return assert(true, true, "test_cv_rotate");
}

int test_cv_scale()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *scale = CV_SCALE(image, 1.5, CV_RGB(0, 0, 0));

    CV_SAVE(scale, "tests/out/test_cv_scale.png");

    CV_FREE(&image);
    CV_FREE(&scale);
    return assert(true, true, "test_cv_scale");
}

int test_cv_resize()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *resize = CV_RESIZE(image, T(300, 200), CV_RGB(0, 0, 0));

    CV_SAVE(resize, "tests/out/test_cv_resize.png");

    CV_FREE(&image);
    CV_FREE(&resize);
    return assert(true, true, "test_cv_resize");
}

int test_cv_zoom()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *zoom = CV_ZOOM(image, 1.5, CV_RGB(0, 0, 0));

    CV_SAVE(zoom, "tests/out/test_cv_zoom.png");

    CV_FREE(&image);
    CV_FREE(&zoom);
    return assert(true, true, "test_cv_zoom");
}

int test_cv_translate()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *translate = CV_TRANSLATE(image, T(50, 50), CV_RGB(0, 0, 0));

    CV_SAVE(translate, "tests/out/test_cv_translate.png");

    CV_FREE(&image);
    CV_FREE(&translate);
    return assert(true, true, "test_cv_translate");
}

