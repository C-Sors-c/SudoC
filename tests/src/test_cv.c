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
        sprintf(path, "tests/out/test_load_images_%d.png", i);
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
        sprintf(path, "tests/out/test_load_images_with_matrix_%d.png", i);
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
        sprintf(path, "tests/out/test_cv_load_image_with_matrix_%d.png", i);
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
    int *lines = CV_HOUGH_LINES(processed, 300, &n);
    int *merged = CV_MERGE_LINES(lines, n, 35, &n);

    CV_DRAW_LINES(image, image, merged, n, 1, CV_RGB(255, 0, 0));

    CV_SAVE(image, "tests/out/test_cv_hough_lines.png");

    CV_FREE(&image);
    CV_FREE(&processed);

    FREE(lines);
    FREE(merged);

    return assert(true, true, "test_cv_hough_lines");
}

int test_cv_simplify_hough_lines()
{
    float angle = 15;

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
    int *simplified = CV_MERGE_LINES(lines, n, 30, &n2);
    float orientation = CV_ORIENTATION(simplified, n2);
    printf("orientation: %f\n", orientation);
    printf("n: %d\n", n);
    printf("n2: %d\n", n2);

    // pass 2
    Image *copy = CV_COPY(image);
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
    int *simplified2 = CV_MERGE_LINES(lines2, n3, 33, &n4);
    // CV_DRAW_LINES(dst, dst, clean_lines, n5, 2, CV_RGB(255, 0, 0));

    CV_SAVE(dst, "tests/out/test_cv_simplified_hough_lines.png");

    CV_FREE(&image);
    CV_FREE(&z1);
    CV_FREE(&r1);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&canny);
    CV_FREE(&dst);

    FREE(lines);
    FREE(simplified);
    FREE(lines2);
    FREE(simplified2);

    return assert(true, true, "test_cv_simplify_hough_lines");
}

int test_cv_intersection()
{
    Image *image = CV_LOAD("tests/samples/sudoku.png", RGB);
    Image *z1 = CV_ZOOM(image, NULL, 1.05, 0xffffff);
    Image *gray = CV_RGB_TO_GRAY(image, NULL);
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, 3, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, 3);
    Image *canny = CV_CANNY(blur, NULL, 0.05, 0.15);
    Image *otsu = CV_OTSU(sharp, NULL);
    Image *z2 = CV_ZOOM(canny, NULL, 1.05, 0xffffff);
    // pass 1
    int n = 0;
    int n2 = 0;
    int n4 = 0;
    int *lines = CV_HOUGH_LINES(z2, 200, &n);
    int *simplified = CV_MERGE_LINES(lines, n, 30, &n2);
    int *intersections = CV_GRID_INTERSECTION(z2, simplified, n2, &n4);
    int *sorted_intersections = CV_SORT_INTERSECTIONS(intersections, n4);

    if (n4 < 100)
    {
        printf("wrong number of interserctions n4: %d\n", n4);
        return assert(false, true, "test_cv_intersection");
    }
    // draw points
    int i;
    for (i = 0; i < n4; i++)
    {
        int x = sorted_intersections[i * 2];
        int y = sorted_intersections[i * 2 + 1];
        printf("x: %d, y: %d\n", x, y);
        CV_DRAW_POINT(z1, z1, x, y, 10, CV_RGB(0, 0, 255));
    }

    printf("intersections: %d\n", n4);

    CV_SAVE(z1, "tests/out/test_cv_intersection.png");

    CV_FREE(&image);
    CV_FREE(&z1);
    CV_FREE(&z2);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&canny);
    CV_FREE(&otsu);

    FREE(lines);
    FREE(simplified);
    FREE(intersections);
    FREE(sorted_intersections);

    return assert(true, true, "test_cv_intersection");
}

int test_cv_boxes()
{
    Image *image = CV_LOAD("tests/samples/sudoku.png", RGB);
    Image *z1 = CV_ZOOM(image, NULL, 1.05, 0xffffff);
    Image *gray = CV_RGB_TO_GRAY(image, NULL);
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, 3, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, 3);
    Image *canny = CV_CANNY(blur, NULL, 0.05, 0.15);
    Image *otsu = CV_OTSU(sharp, NULL);
    Image *z2 = CV_ZOOM(canny, NULL, 1.05, 0xffffff);
    // pass 1
    int n = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0;
    int *lines = CV_HOUGH_LINES(z2, 200, &n);
    int *simplified = CV_MERGE_LINES(lines, n, 30, &n2);
    int *intersections = CV_GRID_INTERSECTION(z2, simplified, n3, &n4);
    int *sorted_intersections = CV_SORT_INTERSECTIONS(intersections, n4);

    if (n4 < 100)
    {
        printf("wrong number of interserctions n4: %d\n", n4);
        return assert(false, true, "test_cv_intersection");
    }

    int *boxes = CV_GRID_BOXES(sorted_intersections, n4, &n5);

    printf("boxes: %d\n", n5);
    // draw points
    for (int i = 0; i < n5; i++)
    {
        int x1 = boxes[i * 4];
        int y1 = boxes[i * 4 + 1];
        int x2 = boxes[i * 4 + 2];
        int y2 = boxes[i * 4 + 3];

        int rw = x2 - x1;
        int rh = y2 - y1;

        CV_DRAW_RECT(z1, z1, x1, y1, rw, rh, 2, CV_RGB(0, 0, 255));
    }

    CV_SAVE(z1, "tests/out/test_cv_boxes.png");

    CV_FREE(&image);
    CV_FREE(&z1);
    CV_FREE(&z2);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&canny);
    CV_FREE(&otsu);

    FREE(lines);
    FREE(simplified);
    FREE(intersections);
    FREE(sorted_intersections);

    return assert(true, true, "test_cv_boxes");
}

int test_cv_save_boxes()
{
    Image *image = CV_LOAD("tests/samples/sudoku2.png", RGB);
    Image *copy = CV_COPY(image);
    Image *gray = CV_RGB_TO_GRAY(image, NULL);

    int k = 5;
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, k, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, k);
    Image *threshold = CV_ADAPTIVE_THRESHOLD(sharp, NULL, k, 0.5, 0.5);

    Image *not = CV_NOT(threshold, NULL);
    Image *dilated = CV_DILATE(not, NULL, 3);
    Image *eroded = CV_ERODE(dilated, NULL, 3);

    Image *processed = CV_COPY(eroded);

    int n = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0;

    int *lines = CV_HOUGH_LINES(processed, 200, &n);
    int *simplified = CV_MERGE_LINES(lines, n, 30, &n2);
    int *intersections = CV_GRID_INTERSECTION(processed, simplified, n3, &n4);
    int *sorted_intersections = CV_SORT_INTERSECTIONS(intersections, n4);
    int *boxes = CV_GRID_BOXES(sorted_intersections, n4, &n5);

    Image *box;

    for (int i = 0; i < n5; i++)
    {
        int x1 = boxes[i * 4];
        int y1 = boxes[i * 4 + 1];
        int x2 = boxes[i * 4 + 2];
        int y2 = boxes[i * 4 + 3];
        box = CV_COPY_REGION(image, x1, y1, x2, y2);

        char path[100];
        sprintf(path, "tests/out/box/test_cv_save_boxes_%d.png", i);
        CV_SAVE(box, path);
    }

    for (int i = 0; i < n5; i += 2)
    {
        int x1 = boxes[i * 4 + 0] + 0;
        int y1 = boxes[i * 4 + 1] + 0;
        int x2 = boxes[i * 4 + 2] - 0;
        int y2 = boxes[i * 4 + 3] - 0;

        int rw = x2 - x1;
        int rh = y2 - y1;

        CV_DRAW_RECT(image, image, x1, y1, rw, rh, 1, CV_RGB(255, 0, 0));
    }

    CV_SAVE(image, "tests/out/test_cv_save_boxes.png");

    CV_FREE(&image);
    CV_FREE(&copy);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&threshold);
    CV_FREE(&not );
    CV_FREE(&dilated);
    CV_FREE(&eroded);
    CV_FREE(&processed);
    CV_FREE(&box);

    FREE(lines);
    FREE(simplified);
    FREE(intersections);
    FREE(sorted_intersections);

    return assert(true, true, "test_cv_boxes");
}

int test_cv_find_countours()
{
    Image *image = CV_LOAD("tests/samples/sudoku15.png", RGB);

    Image *g = CV_RGB_TO_GRAY(image, image);

    Image *gray = CV_RGB_TO_GRAY(image, NULL);

    int k = 5;
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, k, 1);

    Image *sharp = CV_SHARPEN(blur, NULL, k * 2);
    Image *threshold = CV_ADAPTIVE_THRESHOLD(sharp, NULL, k, 0.5, 0.5);

    Image *not = CV_NOT(gray, NULL);

    // Image *sobel = CV_SOBEL(not, NULL);
    // Image *nonmax = CV_NON_MAX_SUPPRESSION(sobel, NULL, NULL, NULL);

    Image *dilated = CV_DILATE(not, NULL, 1);
    Image *eroded = CV_ERODE(dilated, NULL, 1);

    Image *blur2 = CV_GAUSSIAN_BLUR(eroded, NULL, 5, 5);
    Image *canny = CV_CANNY(blur2, NULL, 0.1, 0.9);

    int nlines = 0;
    int *lines = CV_HOUGH_LINES(not, 30, &nlines);

    int nsimplified = 0;
    int *simplified = CV_MERGE_LINES(lines, nlines, 30, &nsimplified);
    // float orientation = CV_ORIENTATION(simplified, nsimplified);
    CV_DRAW_LINES(image, image, simplified, nsimplified, 1, CV_RGB(255, 0, 0));

    CV_SAVE(image, "tests/out/test_cv_find_countours.png");

    // int nn = 0;
    // int *coords = CV_FIND_LARGEST_CONTOUR(processed, &nn);

    for (int i = 0; i < nlines; i++)
    {
        int rho = lines[i * 2];
        int theta = lines[i * 2 + 1];

        printf("rho: %d, theta: %d\n", rho, theta);
    }

    printf("nlines: %d, nsimplified: %d\n", nlines, nsimplified);

    for (int i = 0; i < nsimplified; i++)
    {
        int rho = simplified[i * 2];
        int theta = simplified[i * 2 + 1];

        printf("rho: %d, theta: %d\n", rho, theta);
    }

    // printf("coords: %d\n", nn);

    // for (int i = 0; i < nn; i++)
    // {
    //     int x = coords[i * 2];
    //     int y = coords[i * 2 + 1];
    //     CV_DRAW_POINT(image, x, y, 2, CV_RGB(0, 0, 255));
    // }

    CV_FREE(&image);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&threshold);
    CV_FREE(&not );
    CV_FREE(&dilated);
    CV_FREE(&eroded);
    // CV_FREE(&canny);
    // CV_FREE(&processed);
    // CV_FREE(&draw);

    // FREE(coords);
    // FREE(lines);
    // FREE(simplified);

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

int test_cv_rotate()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *rotated = CV_ROTATE(image, NULL, 45.0, 0x000000);

    CV_SAVE(rotated, "tests/out/test_cv_rotate.png");

    CV_FREE(&image);
    CV_FREE(&rotated);
    return assert(true, true, "test_cv_rotate");
}

int test_cv_resize()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *resized = CV_SCALE(image, NULL, 0.5);

    CV_SAVE(resized, "tests/out/test_cv_resize.png");

    CV_FREE(&image);
    CV_FREE(&resized);
    return assert(true, true, "test_cv_resize");
}

int test_cv_zoom()
{
    Image *image = CV_LOAD("tests/samples/lena.png", RGB);
    Image *zoomed = CV_ZOOM(image, NULL, 2.0, 0x000000);

    CV_SAVE(zoomed, "tests/out/test_cv_zoom.png");

    CV_FREE(&image);
    CV_FREE(&zoomed);
    return assert(true, true, "test_cv_zoom");
}

int test_cv_adaptive_threshold()
{
    char *img1 = "tests/samples/sudoku1.jpeg";
    char *img2 = "tests/samples/sudoku2.png";
    char *img3 = "tests/samples/sudoku.png";
    char *img4 = "tests/samples/sudoku1.png";

    Image *image = CV_LOAD(img4, RGB);
    Image *gray = CV_RGB_TO_GRAY(image, NULL);

    int k = 5;
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, k, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, k * 2);
    Image *threshold = CV_ADAPTIVE_THRESHOLD(sharp, NULL, k, 0.5, 0.5);

    Image *not = CV_NOT(threshold, NULL);
    Image *dilated = CV_DILATE(not, NULL, 3);
    Image *eroded = CV_ERODE(dilated, NULL, 3);

    CV_SAVE(eroded, "tests/out/test_cv_adaptive_threshold.png");

    CV_FREE(&image);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&threshold);
    CV_FREE(&not );
    CV_FREE(&dilated);
    CV_FREE(&eroded);

    return assert(true, true, "test_cv_adaptive_threshold");
}

// -------------- TEST S1 ----------------

int test_s1_cv_gaussian_blur()
{
    Image *image = CV_LOAD("tests/samples/sudoku2.png", GRAYSCALE);
    Image *blurred = CV_GAUSSIAN_BLUR(image, NULL, 5, 1);

    CV_SHOW(blurred, "test_s1_cv_gaussian_blur");

    CV_FREE(&image);
    CV_FREE(&blurred);
    return assert(true, true, "test_s1_cv_gaussian_blur");
}

int test_s1_cv_sobel()
{
    Image *image = CV_LOAD("tests/samples/sudoku.png", GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 5, 1);
    Image *sobel = CV_SOBEL(blur, NULL);

    CV_SHOW(sobel, "test_s1_cv_sobel");

    CV_FREE(&image);
    CV_FREE(&blur);
    CV_FREE(&sobel);
    return assert(true, true, "test_s1_cv_sobel");
}

int test_s1_cv_canny()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.png", GRAYSCALE);
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, 5, 1);
    Image *canny = CV_CANNY(blur, NULL, 0.05, 0.06);
    Image *zoom = CV_ZOOM(canny, NULL, 1.01, 0xffffff);
    CV_SHOW(zoom, "test_s1_cv_canny");

    CV_FREE(&image);
    CV_FREE(&blur);
    CV_FREE(&canny);
    CV_FREE(&zoom);
    return assert(true, true, "test_cv_canny");
}

int test_s1_cv_adaptive_threshold()
{
    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", GRAYSCALE);

    int k = 5;
    Image *blur = CV_GAUSSIAN_BLUR(image, NULL, k, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, k * 2);
    Image *threshold = CV_ADAPTIVE_THRESHOLD(sharp, NULL, k, 0.5, 0.5);

    Image *dilated = CV_DILATE(threshold, NULL, 3);
    Image *eroded = CV_ERODE(dilated, NULL, 3);

    CV_SHOW(eroded, "test_cv_adaptative_otsu_dilate_erode");

    CV_FREE(&image);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&threshold);
    CV_FREE(&dilated);
    CV_FREE(&eroded);

    return assert(true, true, "test_cv_adaptive_threshold");
}

int test_s1_cv_hough_lines()
{

    Image *image = CV_LOAD("tests/samples/sudoku1.jpeg", RGB);

    Image *gray = CV_RGB_TO_GRAY(image, NULL);
    int k = 5;
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, k, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, k * 2);
    Image *threshold = CV_ADAPTIVE_THRESHOLD(sharp, NULL, k, 0.5, 0.5);
    Image *canny = CV_CANNY(threshold, NULL, 0.3, 0.5);
    Image *z1 = CV_ZOOM(canny, NULL, 1.05, 0xffffff);

    int n1 = 0;
    int n2 = 0;
    int *lines = CV_HOUGH_LINES(z1, 300, &n1);
    int *simplified = CV_MERGE_LINES(lines, n1, 30, &n2);
    Image *z2 = CV_ZOOM(image, NULL, 1.05, 0xffffff);
    CV_DRAW_LINES(z2, z2, simplified, n2, 2, CV_RGB(255, 0, 0));

    CV_SHOW(z2, "test_s1_cv_hough_lines");

    CV_FREE(&image);
    CV_FREE(&z1);
    CV_FREE(&z2);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&threshold);
    CV_FREE(&canny);

    FREE(lines);
    FREE(simplified);

    return assert(true, true, "test_s1_cv_hough_lines");
}

int test_s1_cv_rotate(int angle)
{
    Image *image = CV_LOAD("tests/samples/sudoku1.png", RGB);
    Image *rotated = CV_ROTATE(image, NULL, angle, CV_RGB(242, 241, 239));

    CV_SAVE(rotated, "tests/out/test_cv_rotate.png");
    CV_SHOW(rotated, "test_s1_cv_rotate");

    CV_FREE(&image);
    CV_FREE(&rotated);
    return assert(true, true, "test_s1_cv_rotate");
}

int test_s1_cv_save_boxes()
{
    Image *image = CV_LOAD("tests/samples/sudoku.png", RGB);
    Image *copy = CV_COPY(image);
    Image *gray = CV_RGB_TO_GRAY(image, NULL);
    int k = 5;
    Image *blur = CV_GAUSSIAN_BLUR(gray, NULL, k, 1);
    Image *sharp = CV_SHARPEN(blur, NULL, k);
    Image *threshold = CV_ADAPTIVE_THRESHOLD(sharp, NULL, k, 0.5, 0.5);
    Image *dilated = CV_DILATE(threshold, NULL, 3);
    Image *eroded = CV_ERODE(dilated, NULL, 3);
    Image *processed = CV_COPY(eroded);

    int n = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0;

    int *lines = CV_HOUGH_LINES(processed, 200, &n);
    int *simplified = CV_MERGE_LINES(lines, n, 30, &n2);
    int *intersections = CV_GRID_INTERSECTION(processed, simplified, n3, &n4);
    int *sorted_intersections = CV_SORT_INTERSECTIONS(intersections, n4);
    int *boxes = CV_GRID_BOXES(sorted_intersections, n4, &n5);

    Image *box;
    for (int i = 0; i < n5; i++)
    {
        int x1 = boxes[i * 4];
        int y1 = boxes[i * 4 + 1];
        int x2 = boxes[i * 4 + 2];
        int y2 = boxes[i * 4 + 3];
        box = CV_COPY_REGION(image, x1, y1, x2, y2);

        char path[100];
        sprintf(path, "./out/box/test_cv_save_boxes_%d.png", i);
        CV_SAVE(box, path);
    }

    for (int i = 0; i < n5; i++)
    {
        int x1 = boxes[i * 4 + 0] + 0;
        int y1 = boxes[i * 4 + 1] + 0;
        int x2 = boxes[i * 4 + 2] - 0;
        int y2 = boxes[i * 4 + 3] - 0;

        int rw = x2 - x1;
        int rh = y2 - y1;

        CV_DRAW_RECT(image, image, x1, y1, rw, rh, 2, CV_RGB(255, 0, 0));
    }

    CV_SHOW(image, "test_s1_cv_save_boxes");

    CV_FREE(&image);
    CV_FREE(&copy);
    CV_FREE(&gray);
    CV_FREE(&blur);
    CV_FREE(&sharp);
    CV_FREE(&threshold);
    CV_FREE(&dilated);
    CV_FREE(&eroded);
    CV_FREE(&processed);
    // CV_FREE(&box);

    FREE(lines);
    FREE(simplified);
    FREE(intersections);
    FREE(sorted_intersections);
    FREE(boxes);

    return assert(true, true, "test_s1_cv_boxes");
}
