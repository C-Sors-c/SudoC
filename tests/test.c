#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../sudoc/include/utils.h"
#include "include/test_cv.h"

#include "include/test_matrix.h"
#include "include/test_nn.h"

// lists of all cv tests
int (*test_cv[])() = {
    // test_cv_load,
    // test_cv_save,
    // test_cv_zeros,
    // test_cv_ones,
    // test_cv_load_folder,
    // test_cv_matrix_from_folder,
    // test_cv_matrix_from_path,
    // test_cv_list_files,
    // test_cv_rgb_to_gray,
    // test_cv_gray_to_rgb,
    // test_cv_get_gaussian_kernel,
    // test_cv_gaussian_blur,
    // test_cv_gaussian_blur_gray,
    // test_cv_sharp,
    // test_cv_sobel,
    // test_cv_canny,
    // test_cv_otsu,
    // test_cv_adaptive_threshold,
    // test_cv_dilate,
    // test_cv_erode,
    // test_cv_hough_lines,
    // test_cv_hough_lines_merged,
    // test_cv_intersections,
    // test_cv_boxes,
    // test_cv_save_boxes,
    // test_cv_find_countours,
    // test_cv_draw,
    // test_cv_rotate,
    // test_cv_resize,
    //test_cv_zoom, // TODO: fix zoom
    // test_s1_cv_gaussian_blur,
    // test_s1_cv_sobel,
    // test_s1_cv_canny,
    // test_s1_cv_hough_lines,
    // test_s1_cv_save_boxes,
};

// lists of all matrix tests
int (*tests_matrix[])() = {
    // test_matrix_add,
    // test_matrix_subtract,
    // test_matrix_multiply,
    // test_matrix_multiply_scalar,
    // test_matrix_transpose,
    // test_matrix_map_function,
    test_matrix_get_perspective_transformation,
    // test_matrix4_init,
    // test_matrix4_add,
    // test_matrix4_subtract,
    // test_matrix4_multiply_scalar,
    // test_matrix4_transpose,
    // test_matrix4_map_function,
    // test_matrix4_convolve,
    // test_matrix4_add_bias,
    // test_matrix4_sum_rows,
};

int (*tests_nn[])() = {
    // test_nnxor,
    // test_nnxor_load,
};

int main()
{
    // start timer
    clock_t begin = clock();

    // run all tests
    int failed = 0;
    int test_count = 0;

    // MATRIX tests
    printf(YELLOW "Running MATRIX tests..." RESET "\n");
    int test_matrix_count = sizeof(tests_matrix) / sizeof(tests_matrix[0]);
    test_count += test_matrix_count;
    for (int i = 0; i < test_matrix_count; i++)
        failed += tests_matrix[i]();
    printf("\n");

    // NN tests
    printf(YELLOW "Running NN tests..." RESET "\n");
    int test_nn_count = sizeof(tests_nn) / sizeof(tests_nn[0]);
    test_count += test_nn_count;
    for (int i = 0; i < test_nn_count; i++)
        failed += tests_nn[i]();
    printf("\n");

    // CV tests
    printf(YELLOW "Running CV tests..." RESET "\n");
    int test_cv_count = sizeof(test_cv) / sizeof(test_cv[0]);
    test_count += test_cv_count;
    for (int i = 0; i < test_cv_count; i++)
        failed += test_cv[i]();

    // failed += test_s1_cv_rotate(-35);
    printf("\n");

    // stop timer
    clock_t end = clock();
    float time_spent = (float)(end - begin) / CLOCKS_PER_SEC;

    printf("Ran %d tests in %f seconds. %d failed.\n", test_count, time_spent, failed);

    if (failed)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}