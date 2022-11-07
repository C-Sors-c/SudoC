#pragma once

int test_cv_load();
int test_cv_save();
int test_cv_zeros();
int test_cv_ones();
int test_cv_load_folder();
int test_cv_matrix_from_folder();
int test_cv_matrix_from_path();
int test_cv_list_files();
int test_cv_rgb_to_gray();
int test_cv_gray_to_rgb();
int test_cv_get_gaussian_kernel();
int test_cv_gaussian_blur();
int test_cv_gaussian_blur_gray();
int test_cv_sharp();
int test_cv_sobel();
int test_cv_canny();
int test_cv_otsu();
int test_cv_dilate();
int test_cv_erode();
int test_cv_hough_lines();
int test_cv_simplify_hough_lines();
int test_cv_adaptative_threshold();
int test_cv_intersection();
int test_cv_boxes();
int test_cv_save_boxes();
int test_cv_draw();
int test_cv_rotate();
int test_cv_resize();
int test_cv_zoom();
int test_cv_adaptive_threshold();
int test_cv_find_countours();

// test s1
int test_s1_cv_gaussian_blur();
int test_s1_cv_sobel();
int test_s1_cv_canny();
int test_s1_cv_adaptive_threshold();
int test_s1_cv_hough_lines();
int test_s1_cv_rotate(int angle);
int test_s1_cv_save_boxes();