#include "../include/cv.h"
#include <math.h>

#pragma region Image

/// @brief Free an image and set the pointer to NULL. It has float free protection.
/// @param image The image to free.
void CV_FREE(Image **image)
{
    if (*image != NULL)
    {
        FREE((*image)->data);
        FREE(*image);
    }
}

/// @brief Create a new image. The image is initialized with random values between 0 and 1 (malloc).
/// @param channels The number of channels.
/// @param height The height of the image.
/// @param width The width of the image.
/// @return The new image.
Image *CV_INIT(int channels, int height, int width)
{
    Image *image = malloc(sizeof(Image));
    ASSERT_PTR(image);

    image->c = channels;
    image->w = width;
    image->h = height;

    image->data = malloc(channels * height * width * sizeof(pixel_t));
    ASSERT_PTR(image->data);

    return image;
}

/// @brief Create a new black image. The image is initialized with zeros.
/// @param channels The number of channels.
/// @param height The height of the image.
/// @param width The width of the image.
/// @return The new image.
Image *CV_ZEROS(int channels, int height, int width)
{
    Image *image = malloc(sizeof(Image));
    ASSERT_PTR(image);

    image->c = channels;
    image->w = width;
    image->h = height;

    image->data = calloc(channels * height * width, sizeof(pixel_t));
    ASSERT_PTR(image->data);

    return image;
}

/// @brief Create a new white image. The image is initialized with ones.
/// @param channels The number of channels.
/// @param height The height of the image.
/// @param width The width of the image.
/// @return The new image.
Image *CV_ONES(int channels, int height, int width)
{
    Image *image = CV_INIT(channels, height, width);

    for (int i = 0; i < channels * height * width; i++)
        image->data[i] = 1;

    return image;
}

/// @brief Check if an image is black.
/// @param image The image to check.
/// @return True if the image is black, false otherwise.
bool CV_IS_ZERO(const Image *image)
{
    ASSERT_IMG(image);

    for (int i = 0; i < image->c * image->h * image->w; i++)
        if (image->data[i] != 0)
            return false;

    return true;
}

/// @brief Check if an image is white.
/// @param image The image to check.
/// @return True if the image is white, false otherwise.
bool CV_IS_ONE(const Image *image)
{
    ASSERT_IMG(image);

    for (int i = 0; i < image->c * image->h * image->w; i++)
        if (image->data[i] != 1)
            return false;

    return true;
}

/// @brief Create an exact copy of an image.
/// @param src The image to copy.
/// @return The new image.
Image *CV_COPY(const Image *src)
{
    ASSERT_IMG(src);

    Image *copy = CV_INIT(src->c, src->h, src->w);

    memcpy(copy->data, src->data, src->c * src->h * src->w * sizeof(pixel_t));

    return copy;
}

/// @brief Copy an image from src to dst.
/// @param src The image to copy.
/// @param dst The image to copy to.
void CV_COPY_TO(const Image *src, Image *dst)
{
    ASSERT_IMG(src);

    if (dst == NULL)
    {
        dst = CV_COPY(src);
        return;
    }

    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);
    memcpy(dst->data, src->data, src->c * src->h * src->w * sizeof(pixel_t));
}

/// @brief Create a copy of a particlar region of an image.
/// @param src The image to copy.
/// @param xstart The x coordinate of the top left corner of the region.
/// @param ystart The y coordinate of the top left corner of the region.
/// @param xend The x coordinate of the bottom right corner of the region.
/// @param yend The y coordinate of the bottom right corner of the region.
/// @return The new image.
Image *CV_COPY_REGION(const Image *src, int xstart, int ystart, int xend, int yend)
{
    ASSERT_IMG(src);
    Image *dst = CV_INIT(src->c, yend - ystart, xend - xstart);

    for (int c = 0; c < src->c; c++)
        for (int i = ystart; i < yend; i++)
            for (int j = xstart; j < xend; j++)
                PIXEL(dst, c, i - ystart, j - xstart) = PIXEL(src, c, i, j);

    return dst;
}

/// @brief Open an image in a window.
/// @param image The image to open.
/// @param title The title of the window.
void CV_SHOW(const Image *image, char *title)
{
    ASSERT_IMG(image);

    SDL_Surface *surface = CV_IMG_TO_SURFACE(image);
    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, surface->w, surface->h, SDL_WINDOW_SHOWN);
    ASSERT_PTR(window);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    ASSERT_PTR(renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    ASSERT_PTR(texture);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (1)
        if (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                break;

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(surface);
}

/// @brief Open an image stored in a matrix4 in a window.
/// @param matrix The matrix4 to open.
/// @param title The title of the window.
/// @param index The index of the image to open.
void CV_SHOW_MAT4(const Matrix4 *matrix, char *title, int index)
{
    ASSERT_MAT(matrix);
    ASSERT_MAT4_INDEX(matrix, index);

    Image *image = CV_INIT(matrix->dim2, matrix->dim3, matrix->dim4);
    ASSERT_IMG(image);
    ASSERT_CHANNEL(image, matrix->dim2);

    CV_MAT4_TO_IMG(matrix, image, index);
    CV_SHOW(image, title);
    CV_FREE(&image);
}

/// @brief Convert an SDL_Surface to an Image.
/// @param surface The SDL_Surface to convert.
/// @return The new image.
Image *CV_SURFACE_TO_IMG(SDL_Surface *surface)
{
    SDL_LockSurface(surface);

    int w = surface->w;
    int h = surface->h;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    Image *image = CV_INIT(3, h, w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            Uint32 pixel = pixels[i * w + j];
            Uint8 r, g, b;

            SDL_GetRGB(pixel, format, &r, &g, &b);

            PIXEL(image, 0, i, j) = norm(r / 255.0);
            PIXEL(image, 1, i, j) = norm(g / 255.0);
            PIXEL(image, 2, i, j) = norm(b / 255.0);
        }
    }

    SDL_UnlockSurface(surface);
    return image;
}

/// @brief Convert an Image to an SDL_Surface.
/// @param image The image to convert.
/// @return The new SDL_Surface.
SDL_Surface *CV_IMG_TO_SURFACE(const Image *image)
{
    ASSERT_IMG(image);

    SDL_Surface *surface = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);
    ASSERT_PTR(surface);

    SDL_LockSurface(surface);

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    if (image->c == 3)
    {
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                Uint8 r = norm(PIXEL(image, 0, i, j)) * 255;
                Uint8 g = norm(PIXEL(image, 1, i, j)) * 255;
                Uint8 b = norm(PIXEL(image, 2, i, j)) * 255;

                Uint32 pixel = SDL_MapRGB(format, r, g, b);
                pixels[i * image->w + j] = pixel;
            }
        }
    }
    else if (image->c == 1)
    {
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                Uint8 p = norm(PIXEL(image, 0, i, j)) * 255;
                Uint32 pixel = SDL_MapRGB(format, p, p, p);
                pixels[i * image->w + j] = pixel;
            }
        }
    }

    SDL_UnlockSurface(surface);
    return surface;
}

/// @brief Get an SDL_Surface from an image file.
/// @param path The path to the image file.
/// @return The new SDL_Surface.
SDL_Surface *CV_LOAD_SURFACE(const char *path)
{
    SDL_Surface *tempImage = NULL;
    SDL_Surface *Image = NULL;

    tempImage = IMG_Load(path);
    if (tempImage == NULL)
    {
        DEBUG_INFO;
        errx(1, "Unable to load image %s: %s", path, IMG_GetError());
    }

    Image = SDL_ConvertSurfaceFormat(tempImage, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tempImage);
    return Image;
}

/// @brief Convert an image to a matrix4.
/// @param image The image to convert.
/// @param matrix The matrix4 to fill.
/// @param index The index in the matrix4 where to put the image.
/// @return The new matrix4.
Matrix4 *CV_IMG_TO_MAT4(const Image *src, Matrix4 *dst, int index)
{
    ASSERT_IMG(src);

    if (dst == NULL)
    {
        dst = matrix4_init(1, src->c, src->h, src->w, NULL);
        index = 0;
    }

    ASSERT_MAT(dst);
    ASSERT_MAT4_INDEX(dst, index);

    size_t size = src->c * src->h * src->w;
    float *dst_data = dst->data + index * size;
    float *src_data = src->data;
    memcpy(dst_data, src_data, size * sizeof(float));

    return dst;
}

/// @brief Convert a matrix4 to an image.
/// @param matrix The matrix4 to convert.
/// @param dst The image to fill.
/// @param index The index in the matrix4 where to get the image.
/// @return The new image.
Image *CV_MAT4_TO_IMG(const Matrix4 *src, Image *dst, int index)
{
    ASSERT_MAT(src);
    ASSERT_MAT4_INDEX(src, index);

    if (dst == NULL)
        dst = CV_INIT(src->dim2, src->dim3, src->dim4);

    ASSERT_IMG(dst);
    ASSERT_CHANNEL(dst, src->dim2);

    size_t size = dst->c * dst->h * dst->w;
    float *dst_data = dst->data;
    float *src_data = src->data + index * size;
    memcpy(dst_data, src_data, size * sizeof(float));

    return dst;
}

/// @brief Save an image to a file.
/// @param image
/// @param path
void CV_SAVE(const Image *src, char *path)
{
    ASSERT_IMG(src);

    SDL_Surface *surface = CV_IMG_TO_SURFACE(src);
    IMG_SavePNG(surface, path);

    SDL_FreeSurface(surface);
}

/// @brief Load an image from a file.
/// @param path The path to the image file.
/// @param mode The mode to load the image in (RGB=3 or GRAYSCALE=1).
/// @return The new image.
Image *CV_LOAD(const char *path, int mode)
{
    ASSERT_PTR(path);

    SDL_Surface *surface = CV_LOAD_SURFACE(path);
    Image *image = CV_SURFACE_TO_IMG(surface);
    SDL_FreeSurface(surface);

    if (mode == RGB)
        return image;

    if (mode == GRAYSCALE)
    {
        CV_RGB_TO_GRAY(image, image);
        return image;
    }

    DEBUG_INFO;
    errx(1, RED "Invalid mode %d" RESET "\n", mode);
}

/// @brief List all the files in a directory.
/// @param path The path to the directory.
/// @param count The number of files in the directory. (output)
/// @return The list of files.
char **CV_LIST_DIR(const char *path, int *count)
{
    DIR *dir = opendir(path);
    ASSERT_PTR(dir);

    struct dirent *entry;
    int n = 0;
    while ((entry = readdir(dir)) != NULL)
        if (entry->d_type == 8)
            n++;
    closedir(dir);

    char **files = malloc(n * sizeof(char *));
    ASSERT_PTR(files);

    dir = opendir(path);
    ASSERT_PTR(dir);

    int i = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 8)
        {
            char *file = malloc(strlen(path) + strlen(entry->d_name) + 2);
            snprintf(file, strlen(path) + strlen(entry->d_name) + 2, "%s/%s", path, entry->d_name);

            files[i] = file;
            i++;
        }
    }
    closedir(dir);

    *count = n;
    return files;
}

/// @brief Load a dataset from a directory.
/// @param path The path to the directory.
/// @param count The number of images in the dataset. (output)
/// @param mode The mode to load the images in (RGB=3 or GRAYSCALE=1).
/// @return The list of images.
Image **CV_LOAD_FOLDER(const char *path, int *count, int mode)
{
    ASSERT_PTR(path);
    ASSERT_PTR(count);

    Image **images = malloc(sizeof(Image *));
    *count = 0;

    DIR *dir = opendir(path);
    ASSERT_PTR(dir);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 8)
        {
            char *file = malloc(strlen(path) + strlen(entry->d_name) + 2);
            snprintf(file, strlen(path) + strlen(entry->d_name) + 2, "%s/%s", path, entry->d_name);

            Image *image = CV_LOAD(file, mode);
            images = realloc(images, sizeof(Image *) * (*count + 1));
            images[*count] = image;
            (*count)++;
            free(file);
        }
    }

    closedir(dir);
    return images;
}

/// @brief Load a dataset from a directory, but as a matrix4.
/// @param path The path to the directory.
/// @param count The number of images in the dataset. (output)
/// @param mode The mode to load the images in (RGB=3 or GRAYSCALE=1).
/// @return The matrix4 containing the images.
Matrix4 *CV_LOAD_FOLDER_MAT4(const char *path, int *count, int mode)
{
    ASSERT_PTR(path);
    ASSERT_PTR(count);

    int n = 0;
    Image **images = CV_LOAD_FOLDER(path, &n, mode);

    Matrix4 *matrix = matrix4_init(n, images[0]->c, images[0]->h, images[0]->w, NULL);

    for (int i = 0; i < n; i++)
    {
        CV_IMG_TO_MAT4(images[i], matrix, i);
        CV_FREE(&images[i]);
    }

    free(images);
    *count = n;
    return matrix;
}

/// @brief Load a unique image from a directory and convert it to a matrix4.
/// @param path The path to the image file.
/// @param matrix The matrix4 to fill.
/// @param index The index in the matrix4 where to put the image.
/// @param batch_size The size of the batch if the matrix4 is not already allocated.
/// @param mode The mode to load the image in (RGB=3 or GRAYSCALE=1).
/// @return The matrix4 containing the image.
Matrix4 *CV_LOAD_MAT4(const char *path, Matrix4 *matrix, int index, int batch_size, int mode)
{
    ASSERT_PTR(path);

    Image *image = CV_LOAD(path, mode);
    ASSERT_IMG(image);

    if (matrix == NULL)
    {
        matrix = matrix4_init(batch_size, mode, image->h, image->w, NULL);
        index = 0;
    }

    ASSERT_MAT(matrix);

    matrix = CV_IMG_TO_MAT4(image, matrix, index);

    CV_FREE(&image);
    return matrix;
}

#pragma endregion Image

#pragma region Transform

/// @brief Convert an RGB image to GRAYSCALE.
/// @param src The source image.
/// @param dst The destination image.
/// @return The destination image.
Image *CV_RGB_TO_GRAY(const Image *src, Image *dst)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 3);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(1, src->h, src->w);
    else
        dst->c = 1;
    ASSERT_DIM(dst, 1, src->h, src->w);

    dst->data = realloc(dst->data, dst->c * dst->h * dst->w * sizeof(pixel_t));
    ASSERT_PTR(dst->data);

    for (int i = 0; i < src->h; i++)
    {
        for (int j = 0; j < src->w; j++)
        {
            pixel_t r = PIXEL(tmp, 0, i, j);
            pixel_t g = PIXEL(tmp, 1, i, j);
            pixel_t b = PIXEL(tmp, 2, i, j);

            pixel_t gray = (r + g + b) / 3.0;
            PIXEL(dst, 0, i, j) = norm(gray);
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Convert a GRAYSCALE image to RGB.
/// @param src The source image.
/// @param dst The destination image.
/// @return The destination image.
Image *CV_GRAY_TO_RGB(const Image *src, Image *dst)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(3, src->h, src->w);
    else
        dst->c = 3;
    ASSERT_DIM(dst, 3, src->h, src->w);

    dst->data = realloc(dst->data, dst->c * dst->h * dst->w * sizeof(pixel_t));
    ASSERT_PTR(dst->data);

    for (int i = 0; i < src->h; i++)
    {
        for (int j = 0; j < src->w; j++)
        {
            pixel_t gray = PIXEL(tmp, 0, i, j);
            PIXEL(dst, 0, i, j) = gray;
            PIXEL(dst, 1, i, j) = gray;
            PIXEL(dst, 2, i, j) = gray;
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply a convolution filter to an image
/// @param src The source image
/// @param dst The destination image
/// @param kernel The convolution kernel
/// @return The destination image (dst)
Image *CV_APPLY_FILTER(const Image *src, Image *dst, const Matrix *kernel)
{
    ASSERT_IMG(src);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    ASSERT_MAT(kernel);

    int k = kernel->dim1 / 2;

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                pixel_t sum = 0;
                for (int m = 0; m < kernel->dim1; m++)
                {
                    for (int n = 0; n < kernel->dim2; n++)
                    {
                        int x = i + m - k;
                        int y = j + n - k;

                        if (x < 0 || x >= src->h || y < 0 || y >= src->w)
                            continue;

                        sum += PIXEL(tmp, c, x, y) * MAT(kernel, m, n);
                    }
                }

                PIXEL(dst, c, i, j) = norm(sum);
            }
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Dynamicly compute a Gaussian kernel
/// @param size The size of the kernel
/// @param sigma The sigma of the kernel
/// @return The kernel as a size x size Matrix
Matrix *CV_GET_GAUSSIAN_KERNEL(int size, float sigma)
{
    if (size % 2 == 0)
        ERRX("Kernel size must be odd");

    Matrix *kernel = matrix_init(size, size, NULL);
    ASSERT_MAT(kernel);

    if (sigma == 0)
        sigma = (size - 1) / 2;

    int center = size / 2;
    pixel_t sum = 0.0;

    // calculate kernel
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            pixel_t x = i - center;
            pixel_t y = j - center;

            // calculate kernel value
            pixel_t value = exp(-(x * x + y * y) / (2 * sigma * sigma));
            MAT(kernel, i, j) = value;
            sum += value;
        }
    }

    // normalize kernel
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            MAT(kernel, i, j) /= sum;

    return kernel;
}

/// @brief Apply a Gaussian filter to an image
/// @param src The source image
/// @param dst The destination image
/// @param size The size of the kernel
/// @param sigma The sigma of the kernel
/// @return The destination image (dst)
Image *CV_GAUSSIAN_BLUR(const Image *src, Image *dst, int size, float sigma)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    Matrix *kernel = CV_GET_GAUSSIAN_KERNEL(size, sigma);
    CV_APPLY_FILTER(src, dst, kernel);

    matrix_destroy(kernel);
    return dst;
}

/// @brief Apply a Median filter to an image
/// @param src The source image
/// @param dst The destination image
/// @param size The size of the kernel
/// @return The destination image (dst)
Image *CV_MEDIAN_BLUR(const Image *src, Image *dst, int size)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    Matrix *kernel = matrix_init(size, size, NULL);
    ASSERT_MAT(kernel);

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            MAT(kernel, i, j) = 1.0 / (size * size);

    CV_APPLY_FILTER(src, dst, kernel);

    matrix_destroy(kernel);
    return dst;
}

/// @brief Apply a Bilateral filter to an image
/// @param src The source image
/// @param dst The destination image
/// @param size The size of the kernel
/// @param sigma_d The sigma of the distance kernel
/// @param sigma_r The sigma of the range kernel
/// @return The destination image (dst)
Image *CV_BILATERAL_FILTER(const Image *src, Image *dst, int size, float sigma_d, float sigma_r)
{
    ASSERT_IMG(src);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    int k = size / 2;

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                pixel_t sum = 0;
                pixel_t weight = 0;

                for (int m = 0; m < size; m++)
                {
                    for (int n = 0; n < size; n++)
                    {
                        int x = i + m - k;
                        int y = j + n - k;

                        if (x < 0 || x >= src->h || y < 0 || y >= src->w)
                            continue;

                        pixel_t d = PIXEL(tmp, c, x, y) - PIXEL(tmp, c, i, j);
                        pixel_t w = exp(-(d * d) / (2 * sigma_r * sigma_r)) * exp(-(m * m + n * n) / (2 * sigma_d * sigma_d));

                        sum += PIXEL(tmp, c, x, y) * w;
                        weight += w;
                    }
                }

                PIXEL(dst, c, i, j) = norm(sum / weight);
            }
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Dynamicly compute a Sharpen kernel
/// @param sigma The sigma of the kernel (the higher the more sharpen)
/// @return The kernel as a 3x3 Matrix
Matrix *CV_GET_SHARPEN_KERNEL(float sigma)
{
    Matrix *kernel = matrix_init(3, 3, NULL);
    ASSERT_MAT(kernel);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == 1 && j == 1)
                MAT(kernel, i, j) = 4.0 * sigma;
            else if (i == 1 || j == 1)
                MAT(kernel, i, j) = -1.0 * sigma;
            else
                MAT(kernel, i, j) = 0;
        }
    }

    MAT(kernel, 1, 1) += 1;
    return kernel;
}

/// @brief Apply a Sharpen filter to an image
/// @param src The source image
/// @param dst The destination image
/// @param sigma The sigma of the kernel (the higher the more sharpen)
/// @return The destination image (dst)
Image *CV_SHARPEN(const Image *src, Image *dst, float sigma)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    Matrix *kernel = CV_GET_SHARPEN_KERNEL(sigma);
    CV_APPLY_FILTER(src, dst, kernel);

    matrix_destroy(kernel);
    return dst;
}

/// @brief Precomputed a Sobel kernel for the X axis
/// @return The kernel as a 3x3 Matrix
Matrix *CV_GET_SOBEL_KERNEL_X()
{
    float x[] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1};
    Matrix *kernel = matrix_init(3, 3, x);
    ASSERT_MAT(kernel);

    return kernel;
}

/// @brief Precomputed a Sobel kernel for the Y axis
/// @return The kernel as a 3x3 Matrix
Matrix *CV_GET_SOBEL_KERNEL_Y()
{
    float y[] = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1};
    Matrix *kernel = matrix_init(3, 3, y);
    ASSERT_MAT(kernel);

    return kernel;
}

/// @brief Apply a Sobel filter to an image
/// @param src The source image
/// @param dst The destination image
/// @param Gx The destination image for the X axis
/// @param Gy The destination image for the Y axis
/// @return The destination image (dst)
Image *CV_SOBEL_PROCESS(const Image *src, Image *dst, Image *Gx, Image *Gy)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    int null_x = Gx == NULL;
    int null_y = Gy == NULL;

    Matrix *kernel_x = CV_GET_SOBEL_KERNEL_X();
    Matrix *kernel_y = CV_GET_SOBEL_KERNEL_Y();

    if (null_x)
        Gx = CV_APPLY_FILTER(src, NULL, kernel_x);

    if (null_y)
        Gy = CV_APPLY_FILTER(src, NULL, kernel_y);

    ASSERT_DIM(Gx, src->c, src->h, src->w);
    ASSERT_DIM(Gy, src->c, src->h, src->w);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                pixel_t x = PIXEL(Gx, c, i, j);
                pixel_t y = PIXEL(Gy, c, i, j);

                // calculate the magnitude
                PIXEL(dst, c, i, j) = norm(sqrt(x * x + y * y));
            }
        }
    }

    matrix_destroy(kernel_x);
    matrix_destroy(kernel_y);

    if (null_x)
        CV_FREE(&Gx);

    if (null_y)
        CV_FREE(&Gy);

    return dst;
}

/// @brief Apply a Sobel filter to an image
/// @param src The source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_SOBEL(const Image *src, Image *dst)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    CV_SOBEL_PROCESS(src, dst, NULL, NULL);

    return dst;
}

/// @brief Apply Non-Maximum Suppression to an image
/// @param src The source image
/// @param dst The destination image
/// @param Gx Edge image for the X axis
/// @param Gy Edge image for the Y axis
/// @return The destination image (dst)
Image *CV_NON_MAX_SUPPRESSION(const Image *src, Image *dst, Image *Gx, Image *Gy)
{
    ASSERT_IMG(src);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);

    // check if dst_x and dst_y are NULL
    bool null_x = Gx == NULL;
    bool null_y = Gy == NULL;

    Matrix *kernel_x = CV_GET_SOBEL_KERNEL_X();
    Matrix *kernel_y = CV_GET_SOBEL_KERNEL_Y();

    if (null_x)
        Gx = CV_APPLY_FILTER(src, NULL, kernel_x);

    if (null_y)
        Gy = CV_APPLY_FILTER(src, NULL, kernel_y);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 1; i < src->h - 1; i++)
        {
            for (int j = 1; j < src->w - 1; j++)
            {
                pixel_t angle = atan2(PIXEL(Gy, c, i, j), PIXEL(Gx, c, i, j));
                angle *= 180.0 / PI;

                if (angle < 0)
                    angle += 180.0;

                pixel_t q = 1.0;
                pixel_t r = 1.0;

                // https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123
                if ((lte(0, angle) && lt(angle, 22.5)) || (lte(157.5, angle) && lte(angle, 180)))
                {
                    q = PIXEL(tmp, c, i, j + 1);
                    r = PIXEL(tmp, c, i, j - 1);
                }
                else if (lte(22.5, angle) && lt(angle, 67.5))
                {
                    q = PIXEL(tmp, c, i + 1, j - 1);
                    r = PIXEL(tmp, c, i - 1, j + 1);
                }
                else if (lte(67.5, angle) && lt(angle, 112.5))
                {
                    q = PIXEL(tmp, c, i + 1, j);
                    r = PIXEL(tmp, c, i - 1, j);
                }
                else if (lte(112.5, angle) && lt(angle, 157.5))
                {
                    q = PIXEL(tmp, c, i - 1, j - 1);
                    r = PIXEL(tmp, c, i + 1, j + 1);
                }

                if (gte(PIXEL(tmp, c, i, j), q) && gte(PIXEL(tmp, c, i, j), r))
                    PIXEL(dst, c, i, j) = PIXEL(tmp, c, i, j);
                else
                    PIXEL(dst, c, i, j) = 0;
            }
        }
    }

    matrix_destroy(kernel_x);
    matrix_destroy(kernel_y);

    if (null_x)
        CV_FREE(&Gx);

    if (null_y)
        CV_FREE(&Gy);

    CV_FREE(&tmp);

    return dst;
}

/// @brief Apply Hysteresis Thresholding to an image
/// @param src The source image
/// @param dst The destination image
/// @param low The low threshold
/// @param high The high threshold
/// @return The destination image (dst)
Image *CV_THRESHOLD(const Image *src, Image *dst, float low, float high)
{
    ASSERT_IMG(src);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(1, src->h, src->w);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 1; i < src->h - 1; i++)
        {
            for (int j = 1; j < src->w - 1; j++)
            {
                float p = PIXEL(tmp, c, i, j);

                if (p > high)
                    PIXEL(dst, c, i, j) = 1.0;
                else if (p < low)
                    PIXEL(dst, c, i, j) = 0;
                else
                {
                    // this is a weak pixel
                    // check if it is connected to a strong pixel
                    int done = 0;
                    for (int i2 = i - 1; i2 <= i + 1 && !done; i2++)
                        for (int j2 = j - 1; j2 <= j + 1 && !done; j2++)
                            if ((done = PIXEL(dst, c, i2, j2) > high))
                                PIXEL(dst, c, i, j) = 0.5;
                    if (!done)
                        PIXEL(dst, c, i, j) = 0;
                }
            }
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply Hysteresis Thresholding to an image
/// @param src The source image
/// @param dst The destination image
/// @param weak The weak threshold
/// @param strong The strong threshold
/// @return The destination image (dst)
Image *CV_HYSTERISIS(const Image *src, Image *dst, float weak, float strong)
{
    ASSERT_IMG(src);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    // secures that weak < strong
    if (weak > strong)
    {
        float t = weak;
        weak = strong;
        strong = t;
    }

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 1; i < src->h - 1; i++)
        {
            for (int j = 1; j < src->w - 1; j++)
            {
                if (eq(PIXEL(tmp, c, i, j), weak))
                {
                    if ((eq(PIXEL(tmp, c, i + 1, j - 1), strong)) ||
                        (eq(PIXEL(tmp, c, i + 1, j + 0), strong)) ||
                        (eq(PIXEL(tmp, c, i + 1, j + 1), strong)) ||
                        (eq(PIXEL(tmp, c, i + 0, j - 1), strong)) ||
                        (eq(PIXEL(tmp, c, i + 0, j + 1), strong)) ||
                        (eq(PIXEL(tmp, c, i - 1, j - 1), strong)) ||
                        (eq(PIXEL(tmp, c, i - 1, j + 0), strong)) ||
                        (eq(PIXEL(tmp, c, i - 1, j + 1), strong)))
                        PIXEL(dst, 0, i, j) = strong;
                    else
                        PIXEL(dst, 0, i, j) = 0;
                }
            }
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply Canny Edge Detection to an image
/// @param src The source image
/// @param dst The destination image
/// @param low The low threshold
/// @param high The high threshold
/// @return The destination image (dst)
Image *CV_CANNY(const Image *src, Image *dst, float low, float high)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_IMG(dst);

    Matrix *kernel_x = CV_GET_SOBEL_KERNEL_X();
    Matrix *kernel_y = CV_GET_SOBEL_KERNEL_Y();

    Image *Gx = CV_APPLY_FILTER(src, NULL, kernel_x);
    Image *Gy = CV_APPLY_FILTER(src, NULL, kernel_y);

    CV_SOBEL_PROCESS(src, dst, Gx, Gy);

    CV_NON_MAX_SUPPRESSION(dst, dst, Gx, Gy);
    CV_THRESHOLD(dst, dst, low, high);
    CV_HYSTERISIS(dst, dst, 0.5, 1.0);

    matrix_destroy(kernel_x);
    matrix_destroy(kernel_y);

    CV_FREE(&Gx);
    CV_FREE(&Gy);

    return dst;
}

/// @brief Caalculate the Otsu Threshold for an image
/// @param src The source image
/// @return The threshold
pixel_t CV_OTSU_THRESHOLD(const Image *src)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    int histogram[256] = {0};
    int N = src->h * src->w;
    float Nf = (float)N;

    for (int i = 0; i < N; i++)
    {
        float p = src->data[i] * 255.0;
        histogram[(int)p]++;
    }

    float sum = 0;
    for (int i = 0; i < 256; i++)
        sum += i * histogram[i];

    float sumB = 0;
    float q1 = 0;
    float q2 = 0;

    float var_max = 0;
    float threshold = 0;

    for (int t = 0; t < 256; t++)
    {
        q1 += (float)histogram[t];
        if (q1 == 0)
            continue;

        if (q1 == N)
            break;

        q2 = Nf - q1;

        sumB += t * histogram[t];
        float mu1 = sumB / q1;
        float mu2 = (sum - sumB) / q2;

        float sigma = q1 * q2 * ((mu1 - mu2) * (mu1 - mu2));

        if (sigma > var_max)
        {
            var_max = sigma;
            threshold = t;
        }
    }

    return threshold / 255.0;
}

/// @brief Apply Otsu Thresholding to an image
/// @param src The source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_OTSU(const Image *src, Image *dst)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    float threshold = CV_OTSU_THRESHOLD(src);

    for (int h = 0; h < src->h; h++)
    {
        for (int w = 0; w < src->w; w++)
        {
            float p = PIXEL(tmp, 0, h, w);
            if (p > threshold)
                PIXEL(dst, 0, h, w) = 1;
            else
                PIXEL(dst, 0, h, w) = 0;
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply a combination of Otsu and Local Thresholding to an image. We get better results than Otsu alone.
/// @param src The source image
/// @param dst The destination image
/// @param block_size The size of the local block
/// @param otsu_weight The weight of the Otsu threshold
/// @param c The constant to subtract from the mean
/// @return The destination image (dst)
Image *CV_ADAPTIVE_THRESHOLD(const Image *src, Image *dst, int block_size, float otsu_weight, float c)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    if (block_size % 2 == 0)
        block_size++;

    if (otsu_weight < 0)
        otsu_weight = 0;
    else if (otsu_weight > 1)
        otsu_weight = 1;

    float otsu = CV_OTSU_THRESHOLD(src);

    float weight = 1.0 - otsu_weight;
    Image *mean = CV_GAUSSIAN_BLUR(src, NULL, block_size, 1);

    for (int h = 0; h < src->h; h++)
    {
        for (int w = 0; w < src->w; w++)
        {
            float p = PIXEL(tmp, 0, h, w);
            float m = PIXEL(mean, 0, h, w);

            float mcs = m - c * sqrt(m);
            float g2 = weight * weight;

            // magic formula i invented
            float threshold = otsu_weight * (2 * otsu + (m - c * sqrt(m)) * weight * weight);

            if (p > threshold)
                PIXEL(dst, 0, h, w) = 1;
            else
                PIXEL(dst, 0, h, w) = 0;
        }
    }

    // matrix_destroy(kernel);
    CV_FREE(&mean);
    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply an OR operation to two images
/// @param src1 The first source image
/// @param src2 The second source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_OR(const Image *src1, Image *src2, Image *dst)
{
    ASSERT_IMG(src1);
    ASSERT_IMG(src2);

    Image *tmp1 = CV_COPY(src1);
    Image *tmp2 = CV_COPY(src2);

    if (dst == NULL)
        dst = CV_INIT(src1->c, src1->h, src1->w);
    ASSERT_IMG(dst);
    ASSERT_DIM(src2, src1->c, src1->h, src1->w);
    ASSERT_DIM(dst, src1->c, src1->h, src1->w);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(tmp1, c, i, j);
                float p2 = PIXEL(tmp2, c, i, j);

                if (p1 == 1 || p2 == 1)
                    PIXEL(dst, c, i, j) = 1;
                else
                    PIXEL(dst, c, i, j) = 0;
            }
        }
    }

    CV_FREE(&tmp1);
    CV_FREE(&tmp2);
    return dst;
}

/// @brief Apply an AND operation to two images
/// @param src1 The first source image
/// @param src2 The second source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_AND(const Image *src1, Image *src2, Image *dst)
{
    ASSERT_IMG(src1);
    ASSERT_IMG(src2);

    Image *tmp1 = CV_COPY(src1);
    Image *tmp2 = CV_COPY(src2);

    if (dst == NULL)
        dst = CV_INIT(src1->c, src1->h, src1->w);
    ASSERT_IMG(dst);
    ASSERT_DIM(src2, src1->c, src1->h, src1->w);
    ASSERT_DIM(dst, src1->c, src1->h, src1->w);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(tmp1, c, i, j);
                float p2 = PIXEL(tmp2, c, i, j);

                if (p1 == 1 && p2 == 1)
                    PIXEL(dst, c, i, j) = 1;
                else
                    PIXEL(dst, c, i, j) = 0;
            }
        }
    }

    CV_FREE(&tmp1);
    CV_FREE(&tmp2);
    return dst;
}

/// @brief Apply a XOR operation to two images
/// @param src1 The first source image
/// @param src2 The second source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_XOR(const Image *src1, Image *src2, Image *dst)
{
    ASSERT_IMG(src1);
    ASSERT_IMG(src2);

    Image *tmp1 = CV_COPY(src1);
    Image *tmp2 = CV_COPY(src2);

    if (dst == NULL)
        dst = CV_INIT(src1->c, src1->h, src1->w);
    ASSERT_IMG(dst);
    ASSERT_DIM(src2, src1->c, src1->h, src1->w);
    ASSERT_DIM(dst, src1->c, src1->h, src1->w);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(tmp1, c, i, j);
                float p2 = PIXEL(src2, c, i, j);

                if (p1 == 1 && p2 == 0)
                    PIXEL(dst, c, i, j) = 1;
                else if (p1 == 0 && p2 == 1)
                    PIXEL(dst, c, i, j) = 1;
                else
                    PIXEL(dst, c, i, j) = 0;
            }
        }
    }

    CV_FREE(&tmp1);
    CV_FREE(&tmp2);
    return dst;
}

/// @brief Apply a NOT operation to an image
/// @param src The source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_NOT(const Image *src, Image *dst)
{
    ASSERT_IMG(src);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);
    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                float p = PIXEL(tmp, c, i, j);

                if (p == 1)
                    PIXEL(dst, c, i, j) = 0;
                else
                    PIXEL(dst, c, i, j) = 1;
            }
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply a subtraction operation to two images
/// @param src1 The first source image
/// @param src2 The second source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_SUB(const Image *src1, Image *src2, Image *dst)
{
    ASSERT_IMG(src1);
    ASSERT_IMG(src2);

    Image *tmp1 = CV_COPY(src1);
    Image *tmp2 = CV_COPY(src2);

    if (dst == NULL)
        dst = CV_INIT(src1->c, src1->h, src1->w);
    ASSERT_IMG(dst);
    ASSERT_DIM(src2, src1->c, src1->h, src1->w);
    ASSERT_DIM(dst, src1->c, src1->h, src1->w);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(tmp1, c, i, j);
                float p2 = PIXEL(tmp2, c, i, j);

                float p = p1 - p2;
                if (p < 0)
                    p = 0;

                PIXEL(dst, c, i, j) = p;
            }
        }
    }

    CV_FREE(&tmp1);
    CV_FREE(&tmp2);
    return dst;
}

/// @brief Apply an addition operation to two images
/// @param src1 The first source image
/// @param src2 The second source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_ADD(const Image *src1, Image *src2, Image *dst)
{
    ASSERT_IMG(src1);
    ASSERT_IMG(src2);

    Image *tmp1 = CV_COPY(src1);
    Image *tmp2 = CV_COPY(src2);

    if (dst == NULL)
        dst = CV_INIT(src1->c, src1->h, src1->w);
    ASSERT_IMG(dst);
    ASSERT_DIM(src2, src1->c, src1->h, src1->w);
    ASSERT_DIM(dst, src1->c, src1->h, src1->w);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(tmp1, c, i, j);
                float p2 = PIXEL(tmp2, c, i, j);

                float p = p1 + p2;
                if (p > 1)
                    p = 1;

                PIXEL(dst, c, i, j) = p;
            }
        }
    }

    CV_FREE(&tmp1);
    CV_FREE(&tmp2);
    return dst;
}

/// @brief Apply a dilation operation to an image
/// @param src The source image
/// @param dst The destination image
/// @param k The kernel size
/// @return The destination image (dst)
Image *CV_DILATE(const Image *src, Image *dst, int k)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    int r = k / 2;

    for (int h = 0; h < src->h; h++)
    {
        for (int w = 0; w < src->w; w++)
        {
            float max = 0;

            for (int i = -r; i <= r; i++)
            {
                for (int j = -r; j <= r; j++)
                {
                    if (h + i < 0 || h + i >= src->h || w + j < 0 || w + j >= src->w)
                        continue;

                    float p = PIXEL(tmp, 0, h + i, w + j);

                    if (p > max)
                        max = p;
                }
            }

            PIXEL(dst, 0, h, w) = max;
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply an erosion operation to an image
/// @param src The source image
/// @param dst The destination image
/// @param k The kernel size
/// @return The destination image (dst)
Image *CV_ERODE(const Image *src, Image *dst, int k)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    int r = k / 2;

    for (int h = 0; h < src->h; h++)
    {
        for (int w = 0; w < src->w; w++)
        {
            float min = 1;

            for (int i = -r; i <= r; i++)
            {
                for (int j = -r; j <= r; j++)
                {
                    if (h + i < 0 || h + i >= src->h || w + j < 0 || w + j >= src->w)
                        continue;

                    float p = PIXEL(tmp, 0, h + i, w + j);

                    if (p < min)
                        min = p;
                }
            }

            PIXEL(dst, 0, h, w) = min;
        }
    }

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply an opening operation to an image
/// @param src The source image
/// @param dst The destination image
/// @param k The kernel size
/// @return The destination image (dst)
Image *CV_OPEN(const Image *src, Image *dst, int k)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    CV_ERODE(tmp, tmp, k);
    CV_DILATE(tmp, dst, k);

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply a closing operation to an image
/// @param src The source image
/// @param dst The destination image
/// @param k The kernel size
/// @return The destination image (dst)
Image *CV_CLOSE(const Image *src, Image *dst, int k)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    CV_DILATE(tmp, tmp, k);
    CV_ERODE(tmp, dst, k);

    CV_FREE(&tmp);
    return dst;
}

/// @brief Apply a morphological skeletonization to an image
/// @param src The source image
/// @param dst The destination image
/// @return The destination image (dst)
Image *CV_MORPHOLOGICAL_SKELETON(const Image *src, Image *dst)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *img = CV_COPY(src);

    if (dst == NULL)
        dst = CV_INIT(src->c, src->h, src->w);

    ASSERT_IMG(dst);
    ASSERT_DIM(dst, src->c, src->h, src->w);

    memset(dst->data, 0, dst->w * dst->h * dst->c * sizeof(pixel_t));

    Image *tmp = CV_INIT(src->c, src->h, src->w);
    Image *eroded = CV_INIT(src->c, src->h, src->w);

    while (1)
    {
        CV_ERODE(img, eroded, 3);
        CV_DILATE(eroded, tmp, 3);

        CV_SUB(img, tmp, tmp);
        CV_OR(dst, tmp, dst);

        CV_COPY_TO(eroded, img);

        if (CV_IS_ZERO(img))
            break;
    }

    CV_FREE(&tmp);
    CV_FREE(&eroded);
    CV_FREE(&img);

    return dst;
}

/// @brief Build a uint32 from r, g, b values. (0 <= r, g, b <= 255) and a is skipped
/// @param r The red value
/// @param g The green value
/// @param b The blue value
/// @return The uint32 value
Uint32 CV_RGB(Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 color = 0;
    return color | r << 16 | g << 8 | b;
}

/// @brief Build a normalized pixel value from a uint32
/// @param color The uint32 value
/// @param channel The channel to get the value from
/// @return The normalized pixel value
pixel_t CV_COLOR(Uint32 color, int channel)
{
    if (channel < 0 || channel > 2)
        return 0;

    int r = (color >> (16 - channel * 8)) & 0xff;
    return r / 255.0;
}

/// @brief Draw a single point on an image
/// @param src The source image
/// @param dst The destination image
/// @param x The x coordinate
/// @param y The y coordinate
/// @param width The width of the pixel
/// @param color The color of the pixel
/// @return The destination image (dst)
Image *CV_DRAW_POINT(const Image *src, Image *dst, int x, int y, int width, Uint32 color)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    if (x < 0 || x >= dst->w || y < 0 || y >= dst->h)
        return dst;

    for (int c = 0; c < dst->c; c++)
    {
        for (int i = -width / 2; i <= width / 2; i++)
        {
            for (int j = -width / 2; j <= width / 2; j++)
            {
                if (x + i < 0 || x + i >= dst->w || y + j < 0 || y + j >= dst->h)
                    continue;

                PIXEL(dst, c, y + j, x + i) = CV_COLOR(color, c);
            }
        }
    }

    return dst;
}

/// @brief Draw a line on an image
/// @param src The source image
/// @param dst The destination image
/// @param x1 The x coordinate of the first point
/// @param y1 The y coordinate of the first point
/// @param x2 The x coordinate of the second point
/// @param y2 The y coordinate of the second point
/// @param width The width of the line
/// @param color The color of the line
/// @return The destination image (dst)
Image *CV_DRAW_LINE(const Image *src, Image *dst, int x1, int y1, int x2, int y2, int width, Uint32 color)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        for (int i = 0; i < width; i++)
            for (int j = 0; j < width; j++)
                CV_DRAW_POINT(src, dst, x1 + i, y1 + j, width, color);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err = err - dy;
            x1 = x1 + sx;
        }
        if (e2 < dx)
        {
            err = err + dx;
            y1 = y1 + sy;
        }
    }

    return dst;
}

/// @brief Draw a rectangle on an image
/// @param src The source image
/// @param dst The destination image
/// @param x The x coordinate of the top left corner
/// @param y The y coordinate of the top left corner
/// @param w The width of the rectangle
/// @param h The height of the rectangle
/// @param width The width of the rectangle
/// @param color The color of the rectangle
/// @return The destination image (dst)
Image *CV_DRAW_RECT(const Image *src, Image *dst, int x, int y, int w, int h, int width, Uint32 color)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    CV_DRAW_LINE(src, dst, x, y, x + w, y, width, color);
    CV_DRAW_LINE(src, dst, x + w, y, x + w, y + h, width, color);
    CV_DRAW_LINE(src, dst, x + w, y + h, x, y + h, width, color);
    CV_DRAW_LINE(src, dst, x, y + h, x, y, width, color);

    return dst;
}

/// @brief Draw a circle on an image
/// @param src The source image
/// @param dst The destination image
/// @param x The x coordinate of the center
/// @param y The y coordinate of the center
/// @param r The radius of the circle
/// @param width The width of the circle
/// @param color The color of the circle
/// @return
Image *CV_DRAW_CIRCLE(const Image *src, Image *dst, int x, int y, int r, int width, Uint32 color)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int _x = 0;
    int _y = r;

    CV_DRAW_POINT(src, dst, x, y + r, width, color);
    CV_DRAW_POINT(src, dst, x, y - r, width, color);
    CV_DRAW_POINT(src, dst, x + r, y, width, color);
    CV_DRAW_POINT(src, dst, x - r, y, width, color);

    while (_x < _y)
    {
        if (f >= 0)
        {
            _y--;
            ddF_y += 2;
            f += ddF_y;
        }
        _x++;
        ddF_x += 2;
        f += ddF_x;

        CV_DRAW_POINT(src, dst, x - _x, y + _y, width, color);
        CV_DRAW_POINT(src, dst, x + _x, y + _y, width, color);
        CV_DRAW_POINT(src, dst, x + _x, y - _y, width, color);
        CV_DRAW_POINT(src, dst, x - _x, y - _y, width, color);
        CV_DRAW_POINT(src, dst, x + _y, y + _x, width, color);
        CV_DRAW_POINT(src, dst, x - _y, y + _x, width, color);
        CV_DRAW_POINT(src, dst, x + _y, y - _x, width, color);
        CV_DRAW_POINT(src, dst, x - _y, y - _x, width, color);
    }

    return dst;
}

/// @brief Draw a numerical digit on an image
/// @param src The source image
/// @param dst The destination image
/// @param x The x coordinate of the top left corner
/// @param y The y coordinate of the top left corner
/// @param digit The digit to draw (0-9)
/// @param width The width of the digit
/// @param color The color of the digit
/// @return The destination image (dst)
Image *CV_DRAW_DIGIT(const Image *src, Image *dst, int x, int y, int digit, int width, Uint32 color)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    if (digit < 0 || digit > 9)
    {
        DEBUG_INFO;
        ERRX("CV_DRAW_DIGIT: digit must be between 0 and 9");
    }

    int data[10][7] = {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1}, // 9
    };

    float dx = (float)width / 1.47;
    width = (int)dx;
    int bar_w = width / 2;

    for (int i = 0; i < 7; i++)
    {
        if (data[digit][i] == 1)
        {
            switch (i)
            {
            case 0:
                CV_DRAW_LINE(src, dst, x, y, x + width, y, bar_w, color);
                break;
            case 1:
                CV_DRAW_LINE(src, dst, x + width, y, x + width, y + width, bar_w, color);
                break;
            case 2:
                CV_DRAW_LINE(src, dst, x + width, y + width, x + width, y + width * 2, bar_w, color);
                break;
            case 3:
                CV_DRAW_LINE(src, dst, x, y + width * 2, x + width, y + width * 2, bar_w, color);
                break;
            case 4:
                CV_DRAW_LINE(src, dst, x, y + width, x, y + width * 2, bar_w, color);
                break;
            case 5:
                CV_DRAW_LINE(src, dst, x, y, x, y + width, bar_w, color);
                break;
            case 6:
                CV_DRAW_LINE(src, dst, x, y + width, x + width, y + width, bar_w, color);
                break;
            }
        }
    }

    return dst;
}

/// @brief Find Lines in an image using the Hough Transform algorithm.
/// @param src The source image
/// @param threshold The threshold value, representing the minimum number of intersections to detect a line
/// @param nlines The number of lines that will be returned.
/// @return An array of lines where 2n is rho and 2n+1 is theta.
int *CV_HOUGH_TRANSFORM(const Image *src, int threshold, int *nlines)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);
    ASSERT_PTR(nlines);

    *nlines = 0;

    int w = src->w;
    int h = src->h;

    int *accumulator = (int *)calloc(w * h * 180, sizeof(int));
    ASSERT_PTR(accumulator);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (PIXEL(src, 0, y, x) == 0)
                continue;

            for (int t = 0; t < 180; t++)
            {
                float theta = (float)t * PI / 180;             // theta in radian
                float rho = (x * cos(theta) + y * sin(theta)); // rho in pixel

                int r = (int)ceil(rho);
                // smart workaround to avoid negative values
                // since the accumulator is a 1D array
                // we need to shift the values to the right
                // or only
                accumulator[t * w * h + r + w * h / 2]++;
            }
        }
    }

    int *lines = (int *)malloc(sizeof(int) * w * h * 180 * 2);
    ASSERT_PTR(lines);

    // we fill the lines array
    int n = 0;
    for (int i = 0; i < w * h * 180; i++)
    {
        if (accumulator[i] < threshold)
            continue;

        lines[n * 2] = i % (w * h) - w * h / 2; // rho
        lines[n * 2 + 1] = i / (w * h);         // theta
        n++;
    }

    if (n == 0)
    {
        FREE(accumulator);
        FREE(lines);
        return NULL;
    }

    int *rlines = (int *)realloc(lines, sizeof(int) * n * 2);
    if (rlines == NULL)
    {
        FREE(accumulator);
        FREE(lines);
        return NULL;
    }
    else
        lines = rlines;

    // sort the lines by rho
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (lines[i * 2] > lines[j * 2])
            {
                int tmp = lines[i * 2];
                lines[i * 2] = lines[j * 2];
                lines[j * 2] = tmp;

                tmp = lines[i * 2 + 1];
                lines[i * 2 + 1] = lines[j * 2 + 1];
                lines[j * 2 + 1] = tmp;
            }
        }
    }

    // cleanup and return
    *nlines = n;
    FREE(accumulator);
    return lines;
}

/// @brief Merge lines that are close to each other.
/// @param lines The lines array
/// @param nlines The number of lines
/// @param threshold The threshold value to merge lines
/// @param n The number of merged lines
/// @return An array of simplified lines where 2n is rho and 2n+1 is theta.
int *CV_MERGE_LINES(int *lines, int nlines, int threshold, int *n)
{
    ASSERT_PTR(n);
    *n = 0;

    if (lines == NULL)
        return NULL;

    int *merged_lines = (int *)malloc(sizeof(int) * nlines * 2);
    memset(merged_lines, 0, sizeof(int) * nlines * 2);

    int j = 0;
    for (int i = 0; i < nlines * 2; i += 2)
    {
        int rho = lines[i];
        int theta = lines[i + 1];

        int found = false;
        for (int k = 0; k < j; k += 2)
        {
            if (abs(rho - merged_lines[k]) < threshold && abs(theta - merged_lines[k + 1]) < threshold)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            merged_lines[j] = rho;
            merged_lines[j + 1] = theta;
            j += 2;
        }
    }

    *n = j / 2;
    return merged_lines;
}

/// @brief Return detected lines in an image.
/// @param src The source image
/// @param intersection_threshold The threshold value, representing the minimum number of intersections to detect a line
/// @param merge_threshold The threshold value to eliminate lines that are too close to each other.
/// @param nlines The number of lines that will be returned.
/// @return An array of lines where 2n is rho and 2n+1 is theta.
int *CV_HOUGH_LINES(const Image *src, int intersection_threshold, int merge_threshold, int *nlines)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    int *lines = CV_HOUGH_TRANSFORM(src, intersection_threshold, nlines);
    if (lines == NULL)
        return NULL;

    int *merged_lines = CV_MERGE_LINES(lines, *nlines, merge_threshold, nlines);
    FREE(lines);

    return merged_lines;
}

/// @brief Draw lines on an image.
/// @param src The source image
/// @param dst The destination image
/// @param lines The lines array
/// @param nlines The number of lines
/// @param weight The weight of the lines
/// @param color The color of the lines
/// @return The destination image
Image *CV_DRAW_LINES(const Image *src, Image *dst, int *lines, int nlines, int weight, Uint32 color)
{
    ASSERT_IMG(src);

    if (dst == NULL)
        dst = CV_COPY(src);
    ASSERT_IMG(dst);

    for (int i = 0; i < nlines; i++)
    {
        int rho = lines[i * 2];
        int theta = lines[i * 2 + 1];

        if (theta == 0)
            CV_DRAW_LINE(src, dst, rho, 0, rho, dst->h, weight, color);
        else
        {
            // y = mx + p
            // m = tan(theta)
            // p = rho / sin(theta)
            // 2000 is an arbitrary value and should be enough to draw the line
            float a = cos(theta * PI / 180.0);
            float b = sin(theta * PI / 180.0);
            float x0 = a * rho;
            float y0 = b * rho;
            float x1 = x0 + 2000 * (-b);
            float y1 = y0 + 2000 * a;
            float x2 = x0 - 2000 * (-b);
            float y2 = y0 - 2000 * a;

            CV_DRAW_LINE(src, dst, x1, y1, x2, y2, weight, color);
        }
    }

    return dst;
}

/// @brief Find the orientation of an image using lines.
/// @param lines The lines array
/// @param nlines The number of lines
/// @return The orientation of the image in degrees.
float CV_ORIENTATION(int *lines, int nlines)
{
    int *histo = (int *)malloc(sizeof(int) * 180);
    memset(histo, 0, sizeof(int) * 180);

    for (int i = 0; i < nlines; i++)
    {
        int theta = lines[i * 2 + 1];
        histo[theta]++;
    }

    int max = 0;
    int max_theta = 0;
    for (int i = 0; i < 180; i++)
    {
        if (histo[i] > max)
        {
            max = histo[i];
            max_theta = i;
        }
    }

    FREE(histo);
    return max_theta;
}

/// @brief Find all intersections between two lines
/// @param width Width of the image
/// @param height Height of the image
/// @param lines Lines to find intersections
/// @param nlines Number of lines
/// @param nintersection Number of intersections
/// @return Array of intersections where 2n and 2n+1 are x and y coordinates
int *CV_INTERSECTIONS_RAW(int *lines, int nlines, int *nintersection)
{
    int *intersection = (int *)malloc(sizeof(int) * nlines * nlines * 2);
    memset(intersection, 0, sizeof(int) * nlines * nlines * 2);

    int j = 0;
    for (int i = 0; i < nlines; i++)
    {
        int rho1 = lines[i * 2];
        int theta1 = lines[i * 2 + 1];

        for (int k = i + 1; k < nlines; k++)
        {
            int rho2 = lines[k * 2];
            int theta2 = lines[k * 2 + 1];

            float a = cos(theta1 * PI / 180.0);
            float b = sin(theta1 * PI / 180.0);
            float c = cos(theta2 * PI / 180.0);
            float d = sin(theta2 * PI / 180.0);

            float det = a * d - b * c;
            if (det == 0)
                continue;

            float x = (d * rho1 - b * rho2) / det;
            float y = (-c * rho1 + a * rho2) / det;

            intersection[j * 2] = (int)x;
            intersection[j * 2 + 1] = (int)y;
            j++;
        }
    }

    *nintersection = j;

    return intersection;
}

/// @brief Sort intersections by x coordinate
/// @param intersections Intersections to sort
/// @param nintersections Number of intersections
/// @return Sorted intersections by x coordinate
int *CV_INTERSECTIONS_SORT(int *intersections, int nintersections)
{
    int *sorted = (int *)malloc(sizeof(int) * nintersections * 2);
    memset(sorted, 0, sizeof(int) * nintersections * 2);

    for (int i = 0; i < nintersections; i++)
    {
        int x = intersections[i * 2];
        int y = intersections[i * 2 + 1];

        int j = 0;
        for (j = 0; j < i; j++)
        {
            int y2 = sorted[j * 2 + 1];

            if (y < y2)
                break;
        }

        for (int k = i; k > j; k--)
        {
            sorted[k * 2] = sorted[(k - 1) * 2];
            sorted[k * 2 + 1] = sorted[(k - 1) * 2 + 1];
        }

        sorted[j * 2] = x;
        sorted[j * 2 + 1] = y;
    }

    return sorted;
}

/// @brief Find all intersections between two lines
/// @param lines Lines to find intersections
/// @param nlines Number of lines
/// @param nintersection Number of intersections
/// @return Array of intersections where 2n and 2n+1 are x and y coordinates
int *CV_INTERSECTIONS(int *lines, int nlines, int *nintersection)
{
    int *intersections = CV_INTERSECTIONS_RAW(lines, nlines, nintersection);
    int *sorted = CV_INTERSECTIONS_SORT(intersections, *nintersection);

    FREE(intersections);

    return sorted;
}

/// @brief Find all boxes in a grid
/// @param intersections Intersections to find boxes
/// @param nintersections Number of intersections
/// @param nboxes Number of boxes
/// @return Array of boxes where 4n, 4n+1, 4n+2 and 4n+3 are x and y coordinates of the 4 corners
int *CV_GRID_BOXES(int *intersections, int nintersections, int *nboxes)
{
    int *boxes = (int *)malloc(sizeof(int) * nintersections * 4);
    memset(boxes, 0, sizeof(int) * nintersections * 4);

    int snbi = sqrt(nintersections);

    int j = 0;
    for (int i = 0; i < nintersections - snbi; i++)
    {
        if ((i + 1) % snbi == 0)
            continue;

        int x1 = intersections[i * 2];
        int y1 = intersections[i * 2 + 1];
        int x4 = intersections[i * 2 + snbi * 2 + 2];
        int y4 = intersections[i * 2 + snbi * 2 + 3];

        boxes[j * 4] = x1;
        boxes[j * 4 + 1] = y1;
        boxes[j * 4 + 2] = x4;
        boxes[j * 4 + 3] = y4;
        j++;
    }

    *nboxes = j;
    return boxes;
}

/// @brief Find the largest connected component in a binary image
/// @param poly The polygon
/// @param npoly Number of points in the polygon
/// @return The area of the polygon
float CV_POLY_AREA(int *poly, int npoly)
{
    float area = 0.0;

    for (int i = 0; i < npoly; i++)
    {
        int j = (i + 1) % npoly;

        area += poly[i * 2] * poly[j * 2 + 1];
        area -= poly[j * 2] * poly[i * 2 + 1];
    }

    area /= 2.0;
    
    if (area < 0)
        area = -area;
    
    return area;
}

/// @brief Find the largest connected component in a binary image
/// @param src The source image
/// @param n The number of rectangles
/// @return An array of rectangles.
int *CV_FIND_MAX_CONTOUR(const Image *src, int *n)
{
    ASSERT_IMG(src);
    ASSERT_CHANNEL(src, 1);

    Image *tmp = CV_COPY(src);

    int w = tmp->w;
    int h = tmp->h;

    int ncontours = 0;
    int *contours = (int *)malloc(sizeof(int) * w * h * 2);
    memset(contours, 0, sizeof(int) * w * h * 2);

    int *visited = (int *)malloc(sizeof(int) * w * h * 2);
    memset(visited, 0, sizeof(int) * w * h * 2);

    int ncontours_out = 0;
    int *contours_out = NULL;

    int maxarea = 0;
    int realarea = 0;
    int nconvex_out = 0;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (PIXEL(tmp, 0, y, x) == 0)
                continue;

            if (visited[y * w + x])
                continue;

            int *stack = (int *)calloc(w * h, sizeof(int));
            int nstack = 0;

            stack[nstack * 2] = x;
            stack[nstack * 2 + 1] = y;
            nstack++;

            while (nstack > 0)
            {
                int x = stack[(nstack - 1) * 2];
                int y = stack[(nstack - 1) * 2 + 1];
                nstack--;

                if (visited[y * w + x])
                    continue;

                visited[y * w + x] = 1;

                contours[ncontours * 2] = x;
                contours[ncontours * 2 + 1] = y;
                ncontours++;

                if (x > 0 && PIXEL(tmp, 0, y, x - 1) == 1 && !visited[y * w + x - 1])
                {
                    stack[nstack * 2] = x - 1;
                    stack[nstack * 2 + 1] = y;
                    nstack++;
                }

                if (x < w - 1 && PIXEL(tmp, 0, y, x + 1) == 1 && !visited[y * w + x + 1])
                {
                    stack[nstack * 2] = x + 1;
                    stack[nstack * 2 + 1] = y;
                    nstack++;
                }

                if (y > 0 && PIXEL(tmp, 0, y - 1, x) == 1 && !visited[(y - 1) * w + x])
                {
                    stack[nstack * 2] = x;
                    stack[nstack * 2 + 1] = y - 1;
                    nstack++;
                }

                if (y < h - 1 && PIXEL(tmp, 0, y + 1, x) == 1 && !visited[(y + 1) * w + x])
                {
                    stack[nstack * 2] = x;
                    stack[nstack * 2 + 1] = y + 1;
                    nstack++;
                }
            }

            FREE(stack);
            
            int nconvex = 0;
            int *convex = CV_CONVEX_HULL(contours, ncontours, &nconvex);
            int polygon_area = CV_POLY_AREA(convex, nconvex);

            if (polygon_area > maxarea || ncontours > realarea)
            {
                maxarea = polygon_area;
                realarea = ncontours;
                nconvex_out = nconvex;

                FREE(contours_out);
                contours_out = (int *)malloc(sizeof(int) * nconvex * 2);
                memcpy(contours_out, convex, sizeof(int) * nconvex * 2);
            }

            ncontours = 0;
            FREE(convex);
        }
    }

    FREE(visited);
    FREE(contours);
    CV_FREE(&tmp);

    *n = nconvex_out;
    return contours_out;
}

/// @brief Apply the Jarvis March algorithm to find the convex hull of a set of points
/// @param points The points to find the convex hull
/// @param n The number of points
/// @param nconvex The number of points in the convex hull
/// @return An array of points in the convex hull
int *CV_CONVEX_HULL(int *points, int n, int *nconvex)
{
    *nconvex = n;

    int *convex = (int *)malloc(sizeof(int) * n * 2);
    memset(convex, 0, sizeof(int) * n * 2);

    int *visited = (int *)malloc(sizeof(int) * n);
    memset(visited, 0, sizeof(int) * n);

    int x = points[0];
    int y = points[1];

    for (int i = 1; i < n; i++)
    {
        if (points[i * 2 + 1] < y || (points[i * 2 + 1] == y && points[i * 2] < x))
        {
            x = points[i * 2];
            y = points[i * 2 + 1];
        }
    }

    int j = 0;
    int k = 0;

    do
    {
        convex[j * 2] = x;
        convex[j * 2 + 1] = y;
        visited[k] = 1;
        j++;

        int x1 = points[0];
        int y1 = points[1];
        int k1 = 0;

        for (int i = 1; i < n; i++)
        {
            if (visited[i])
                continue;

            int x2 = points[i * 2];
            int y2 = points[i * 2 + 1];

            int cross = (x2 - x) * (y1 - y) - (x1 - x) * (y2 - y);

            if (cross < 0)
            {
                x1 = x2;
                y1 = y2;
                k1 = i;
            }
            else if (cross == 0)
            {
                if (sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y)) > sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y)))
                {
                    x1 = x2;
                    y1 = y2;
                    k1 = i;
                }
            }
        }

        x = x1;
        y = y1;
        k = k1;

    } while (x != convex[0] || y != convex[1]);

    *nconvex = j;

    FREE(visited);

    return convex;
}

/// @brief Find a rectangle that fits inside a convex hull
/// @param points The points in the convex hull
/// @param n The number of points in the convex hull
/// @return An array of 4 points in the rectangle
int *CV_GET_RECT_FROM_CONTOUR(int *points, int npoints)
{
    int *rect = (int *)malloc(sizeof(int) * 8);
    memset(rect, 0, sizeof(int) * 8);

    int minx = INT_MAX;
    int miny = INT_MAX;
    int maxx = 0;
    int maxy = 0;

    // Find the bounding box
    for (int i = 0; i < npoints; i++)
    {
        int x = points[i * 2];
        int y = points[i * 2 + 1];

        if (x < minx)
            minx = x;
        if (x > maxx)
            maxx = x;
        if (y < miny)
            miny = y;
        if (y > maxy)
            maxy = y;
    }

    int tlx = 0;
    int tly = 0;
    int trx = 0;
    int try = 0;
    int brx = 0;
    int bry = 0;
    int blx = 0;
    int bly = 0;

    float tl = 0;
    float tr = 0;
    float br = 0;
    float bl = 0;

    // Find the points that are closest to the corners of the bounding box
    for (int i = 0; i < npoints; i++)
    {
        int x = points[i * 2];
        int y = points[i * 2 + 1];

        float dst1 = sqrt((x - minx) * (x - minx) + (y - miny) * (y - miny));
        float dst2 = sqrt((x - maxx) * (x - maxx) + (y - miny) * (y - miny));
        float dst3 = sqrt((x - maxx) * (x - maxx) + (y - maxy) * (y - maxy));
        float dst4 = sqrt((x - minx) * (x - minx) + (y - maxy) * (y - maxy));

        if (dst1 > tl)
        {
            tl = dst1;
            tlx = x;
            tly = y;
        }

        if (dst2 > tr)
        {
            tr = dst2;
            trx = x;
            try = y;
        }

        if (dst3 > br)
        {
            br = dst3;
            brx = x;
            bry = y;
        }

        if (dst4 > bl)
        {
            bl = dst4;
            blx = x;
            bly = y;
        }
    }

    // idk why but coordonates need to be reversed
    rect[4] = tlx;
    rect[5] = tly;
    rect[6] = trx;
    rect[7] = try;
    rect[0] = brx;
    rect[1] = bry;
    rect[2] = blx;
    rect[3] = bly;

    return rect;
}

/// @brief Find the 4 corners of the biggest rectangle in an image
/// @param src1 The source image that will be used to find the contours
/// @param src2 The source image that will be used to find the intersections
/// @return An array of 4 points in the rectangle
int *CV_FIND_SUDOKU_RECT(const Image *src1, const Image *src2)
{
    ASSERT_IMG(src1);
    ASSERT_IMG(src2);
    ASSERT_CHANNEL(src1, 1);
    ASSERT_CHANNEL(src2, 1);
    ASSERT_DIM(src1, src2->c, src2->h, src2->w);

    // Find the biggest contour
    int ncontours = 0;
    int *contours = CV_FIND_MAX_CONTOUR(src1, &ncontours);
    if (ncontours == 0)
        return NULL;

    // Detect lines
    int nlines = 0;
    int s = min(src2->w, src2->h); // get smaller dimension
    int lt = clamp(s / 6, 200, 350); // line threshold
    int *lines = CV_HOUGH_LINES(src2, lt, 25, &nlines); // detect lines
    if (nlines == 0)
        return NULL;

    // Find intersections
    int nintersections = 0;
    int *intersections = CV_INTERSECTIONS(lines, nlines, &nintersections);
    if (nintersections == 0)
        return NULL;

    // Find the points that are close to an intersection
    int *newcontours = (int *)calloc(ncontours * 2, sizeof(int));
    int npoints = 0;
    for (int i = 0; i < ncontours; i++)
    {
        int x = contours[i * 2];
        int y = contours[i * 2 + 1];

        for (int j = 0; j < nintersections; j++)
        {
            int x2 = intersections[j * 2];
            int y2 = intersections[j * 2 + 1];

            if (sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y)) < 20)
            {
                newcontours[npoints * 2] = x;
                newcontours[npoints * 2 + 1] = y;
                npoints++;
                break;
            }
        }
    }

    // Find the 4 corners of the biggest rectangle
    int *rect = NULL;
    rect = CV_GET_RECT_FROM_CONTOUR(newcontours, npoints);
    if (rect == NULL)
        return NULL;

    FREE(contours);
    FREE(newcontours);
    FREE(lines);
    FREE(intersections);

    return rect;
}

/// @brief Apply a perspective transform to an image
/// @param src Source image.
/// @param M 3x3 perspective transformation matrix.
/// @param dsize Size of the output image.
/// @param offset Offset of the transformation in the destination image.
/// @param background Background color in the destination image.
/// @return Destination image.
Image *CV_TRANSFORM(const Image *src, const Matrix *M, Tupple dsize, Tupple offset, Uint32 background)
{
    ASSERT_IMG(src);

    Image *dst = CV_INIT(src->c, dsize.x, dsize.y);

    ASSERT_MAT(M);
    if (M->dim1 != 3 || M->dim2 != 3)
    {
        DEBUG_INFO;
        ERRX("Matrix must be 3x3");
    }

    float m11 = MAT(M, 0, 0);
    float m12 = MAT(M, 0, 1);
    float m13 = MAT(M, 0, 2);

    float m21 = MAT(M, 1, 0);
    float m22 = MAT(M, 1, 1);
    float m23 = MAT(M, 1, 2);

    float m31 = MAT(M, 2, 0);
    float m32 = MAT(M, 2, 1);
    float m33 = MAT(M, 2, 2);

    for (int c = 0; c < dst->c; c++)
    {
        for (int y = 0; y < dst->h; y++)
        {
            for (int x = 0; x < dst->w; x++)
            {
                int xt = x - offset.x;
                int yt = y - offset.y;

                float x1 = (m11 * xt + m12 * yt + m13) / (m31 * xt + m32 * yt + m33);
                float y1 = (m21 * xt + m22 * yt + m23) / (m31 * xt + m32 * yt + m33);

                int x2 = (int)x1;
                int y2 = (int)y1;

                if (x1 < 0 || x1 >= src->w || y1 < 0 || y1 >= src->h)
                    PIXEL(dst, c, y, x) = CV_COLOR(background, c);
                else
                    PIXEL(dst, c, y, x) = PIXEL(src, c, y2, x2);
            }
        }
    }

    return dst;
}

/// @brief Rotate an image
/// @param src Source image
/// @param angle Angle of rotation in degrees
/// @param resize Resize the image to fit the rotated image
/// @param background Background color to fill the empty space
/// @return Rotated image
Image *CV_ROTATE(const Image *src, float angle, bool resize, Uint32 background)
{
    ASSERT_IMG(src);

    float rad = angle * PI / 180.0f;

    float m[9] = {
        cos(rad), -sin(rad), src->w / 2.0f,
        sin(rad), cos(rad), src->h / 2.0f,
        0, 0, 1};

    Matrix *M = matrix_init(3, 3, m);

    int w = src->w;
    int h = src->h;

    if (resize)
    {
        w = (int)(src->w * fabs(cos(rad)) + src->h * fabs(sin(rad)));
        h = (int)(src->w * fabs(sin(rad)) + src->h * fabs(cos(rad)));
    }

    Tupple dsize = {w, h};
    Tupple offset = {w / 2.0f, h / 2.0f};

    Image *dst = CV_TRANSFORM(src, M, dsize, offset, background);

    matrix_destroy(M);

    return dst;
}

/// @brief Scale an image
/// @param src Source image
/// @param factor Scale factor
/// @param background Background color to fill the empty space
/// @return Scaled image
Image *CV_SCALE(const Image *src, float factor, Uint32 background)
{
    ASSERT_IMG(src);

    float m[9] = {
        1.0f / factor, 0, 0,
        0, 1.0f / factor, 0,
        0, 0, 1};

    Matrix *M = matrix_init(3, 3, m);

    Tupple dsize = {src->w * factor, src->h * factor};
    Tupple offset = {0, 0};

    Image *dst = CV_TRANSFORM(src, M, dsize, offset, background);

    matrix_destroy(M);

    return dst;
}

/// @brief Resize an image to a given size
/// @param src Source image
/// @param dsize Destination size
/// @param background Background color to fill the empty space
/// @return Resized image
Image *CV_RESIZE(const Image *src, Tupple dsize, Uint32 background)
{
    ASSERT_IMG(src);

    float x_factor = (float)dsize.x / (float)src->w;
    float y_factor = (float)dsize.y / (float)src->h;

    float ix = 1.0f / x_factor;
    float iy = 1.0f / y_factor;

    float m[9] = {
        iy, 0, 0,
        0, ix, 0,
        0, 0, 1};

    Matrix *M = matrix_init(3, 3, m);

    Tupple offset = {0, 0};

    Image *dst = CV_TRANSFORM(src, M, dsize, offset, background);

    matrix_destroy(M);

    return dst;
}

/// @brief Zoom in/out an image without changing the image size
/// @param src Source image
/// @param factor Zoom factor
/// @param background Background color to fill the empty space
/// @return Zoomed image
Image *CV_ZOOM(const Image *src, float factor, Uint32 background)
{
    ASSERT_IMG(src);

    factor = 1.0f / factor;

    float m[9] = {
        factor, 0, (1 - factor) * src->w / 2.0f,
        0, factor, (1 - factor) * src->h / 2.0f,
        0, 0, 1};

    Matrix *M = matrix_init(3, 3, m);

    Tupple dsize = {src->h, src->w};
    Tupple offset = {0, 0};

    Image *dst = CV_TRANSFORM(src, M, dsize, offset, background);

    matrix_destroy(M);

    return dst;
}

/// @brief Translate an image
/// @param src Source image
/// @param offset Offset
/// @param background Background color to fill the empty space
/// @return Translated image
Image *CV_TRANSLATE(const Image *src, Tupple offset, Uint32 background)
{
    ASSERT_IMG(src);

    float m[9] = {
        1, 0, offset.x,
        0, 1, offset.y,
        0, 0, 1};

    Matrix *M = matrix_init(3, 3, m);

    Tupple dsize = {src->w, src->h};

    Image *dst = CV_TRANSFORM(src, M, dsize, offset, background);

    matrix_destroy(M);

    return dst;
}

#pragma endregion Transform
