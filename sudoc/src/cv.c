#include "../include/cv.h"
#include <math.h>

#pragma region Image

void CV_IMAGE_FREE(Image *image)
{
    if (image != NULL)
    {
        if (image->data != NULL)
            FREE(image->data);
        FREE(image);
    }
}

Image *CV_IMAGE_INIT(int channels, int height, int width)
{
    Image *image = malloc(sizeof(Image));
    CV_CHECK_PTR(image);

    image->c = channels;
    image->w = width;
    image->h = height;

    image->data = malloc(channels * height * width * sizeof(pixel_t));
    CV_CHECK_PTR(image->data);

    return image;
}

Image *CV_ZEROS(int channels, int height, int width)
{
    Image *image = malloc(sizeof(Image));
    CV_CHECK_PTR(image);

    image->c = channels;
    image->w = width;
    image->h = height;

    image->data = calloc(channels * height * width, sizeof(pixel_t));
    CV_CHECK_PTR(image->data);

    return image;
}

Image *CV_ONES(int channels, int height, int width)
{
    Image *image = CV_IMAGE_INIT(channels, height, width);

    for (int i = 0; i < channels * height * width; i++)
        image->data[i] = 1;

    return image;
}

Image *CV_IMAGE_COPY(Image *image)
{
    CV_CHECK_IMAGE(image);
    Image *copy = CV_IMAGE_INIT(image->c, image->h, image->w);
    memcpy(copy->data, image->data, image->c * image->h * image->w * sizeof(pixel_t));

    return copy;
}

void CV_IMAGE_SHOW(Image *image, char *title)
{
    CV_CHECK_IMAGE(image);

    SDL_Surface *surface = CV_SURFACE_FROM_IMAGE(image);
    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, surface->w, surface->h, SDL_WINDOW_SHOWN);
    CV_CHECK_PTR(window);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    CV_CHECK_PTR(renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    CV_CHECK_PTR(texture);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (1)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                break;
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(surface);
}

void CV_IMAGE_SHOW_MATRIX4(Matrix4 *matrix, char *title, int index)
{
    CV_CHECK_PTR(matrix);
    CV_CHECK_PTR(matrix->data);
    CV_CHECK_INDEX(matrix, index);

    Image *image = CV_IMAGE_INIT(matrix->dim2, matrix->dim3, matrix->dim4);
    CV_CHECK_IMAGE(image);
    CV_CHECK_CHANNEL(image, matrix->dim2);

    CV_IMAGE_FROM_MATRIX4(matrix, image, index);
    CV_IMAGE_SHOW(image, title);
    CV_IMAGE_FREE(image);
}

Image *CV_IMAGE_FROM_SURFACE(SDL_Surface *surface)
{
    SDL_LockSurface(surface);

    int w = surface->w;
    int h = surface->h;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    Image *image = CV_IMAGE_INIT(3, h, w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            Uint32 pixel = pixels[i * w + j];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, format, &r, &g, &b);
            PIXEL(image, 0, i, j) = r / 255.0;
            PIXEL(image, 1, i, j) = g / 255.0;
            PIXEL(image, 2, i, j) = b / 255.0;
        }
    }

    SDL_UnlockSurface(surface);
    return image;
}

SDL_Surface *CV_SURFACE_FROM_IMAGE(Image *image)
{
    CV_CHECK_IMAGE(image);

    SDL_Surface *surface = SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);
    CV_CHECK_PTR(surface);
    SDL_LockSurface(surface);

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    if (image->c == 3)
    {
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                Uint8 r = NORM(PIXEL(image, 0, i, j)) * 255;
                Uint8 g = NORM(PIXEL(image, 1, i, j)) * 255;
                Uint8 b = NORM(PIXEL(image, 2, i, j)) * 255;
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
                Uint8 r = NORM(PIXEL(image, 0, i, j)) * 255;
                Uint32 pixel = SDL_MapRGB(format, r, r, r);
                pixels[i * image->w + j] = pixel;
            }
        }
    }

    SDL_UnlockSurface(surface);
    return surface;
}

SDL_Surface *CV_SURFACE_FROM_PATH(char *path)
{
    SDL_Surface *tempImage = NULL;
    SDL_Surface *Image = NULL;

    tempImage = IMG_Load(path);
    if (tempImage == NULL)
    {
        ERROR_INFO;
        errx(1, "Unable to load image %s: %s", path, IMG_GetError());
    }

    Image = SDL_ConvertSurfaceFormat(tempImage, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tempImage);
    return Image;
}

Matrix4 *CV_MATRIX4_FROM_IMAGE(Image *image, Matrix4 *matrix, int index)
{
    CV_CHECK_IMAGE(image);

    if (matrix == NULL)
    {
        matrix = matrix4_init(1, image->c, image->h, image->w, NULL);
        index = 0;
    }

    CV_CHECK_PTR(matrix);
    CV_CHECK_PTR(matrix->data);
    CV_CHECK_INDEX(matrix, index);

    size_t size = image->c * image->h * image->w;
    float *dst = matrix->data + index * size;
    float *src = image->data;
    memcpy(dst, src, size * sizeof(float));

    return matrix;
}

Image *CV_IMAGE_FROM_MATRIX4(Matrix4 *matrix, Image *image, int index)
{
    CV_CHECK_PTR(matrix);
    CV_CHECK_PTR(matrix->data);
    CV_CHECK_INDEX(matrix, index);

    if (image == NULL)
    {
        image = CV_IMAGE_INIT(matrix->dim2, matrix->dim3, matrix->dim4);
    }

    CV_CHECK_IMAGE(image);
    CV_CHECK_CHANNEL(image, matrix->dim2);

    size_t size = image->c * image->h * image->w;
    float *dst = image->data;
    float *src = matrix->data + index * size;
    memcpy(dst, src, size * sizeof(float));

    return image;
}

void CV_SAVE(Image *image, char *path)
{
    CV_CHECK_IMAGE(image);
    SDL_Surface *surface = CV_SURFACE_FROM_IMAGE(image);
    IMG_SavePNG(surface, path);
    SDL_FreeSurface(surface);
}

Image *CV_LOAD(char *path, int mode)
{
    CV_CHECK_PTR(path);

    SDL_Surface *surface = CV_SURFACE_FROM_PATH(path);
    Image *image = CV_IMAGE_FROM_SURFACE(surface);
    SDL_FreeSurface(surface);

    if (mode == CV_RGB)
        return image;

    if (mode == CV_GRAYSCALE)
    {
        Image *gray = CV_RGB_TO_GRAY(image, NULL);
        CV_IMAGE_FREE(image);
        return gray;
    }

    errx(1, "Invalid mode");
}

char **CV_LIST_FILES(char *path, int *count)
{
    DIR *dir = opendir(path);
    CV_CHECK_PTR(dir);

    struct dirent *entry;
    int n = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 8)
        {
            n++;
        }
    }
    closedir(dir);

    char **files = malloc(n * sizeof(char *));
    CV_CHECK_PTR(files);

    dir = opendir(path);
    CV_CHECK_PTR(dir);

    int i = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 8)
        {
            char *file = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(file, "%s/%s", path, entry->d_name);

            files[i] = file;
            i++;
        }
    }
    closedir(dir);

    *count = n;
    return files;
}

Image **CV_LOAD_FOLDER(char *path, int *count, int mode)
{
    CV_CHECK_PTR(path);
    CV_CHECK_PTR(count);

    Image **images = malloc(sizeof(Image *));
    *count = 0;

    DIR *dir = opendir(path);
    CV_CHECK_PTR(dir);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 8)
        {
            char *file = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(file, "%s/%s", path, entry->d_name);

            Image *image = CV_LOAD(file, mode);
            if (image != NULL)
            {
                images = realloc(images, sizeof(Image *) * (*count + 1));
                images[*count] = image;
                (*count)++;
            }
            free(file);
        }
    }

    closedir(dir);
    return images;
}

Matrix4 *CV_MATRIX4_FROM_FOLDER(char *path, int *count, int mode)
{
    CV_CHECK_PTR(path);
    CV_CHECK_PTR(count);

    int n = 0;
    Image **images = CV_LOAD_FOLDER(path, &n, mode);

    Matrix4 *matrix = matrix4_init(n, images[0]->c, images[0]->h, images[0]->w, NULL);

    for (int i = 0; i < n; i++)
    {
        CV_MATRIX4_FROM_IMAGE(images[i], matrix, i);
        CV_IMAGE_FREE(images[i]);
    }

    free(images);
    *count = n;

    return matrix;
}

Matrix4 *CV_MATRIX4_FROM_PATH(char *path, Matrix4 *matrix, int index, int batch_size, int mode)
{
    CV_CHECK_PTR(path);

    Image *image = CV_LOAD(path, mode);
    CV_CHECK_IMAGE(image);

    if (matrix == NULL)
    {
        matrix = matrix4_init(batch_size, mode, image->h, image->w, NULL);
        index = 0;
    }

    CV_CHECK_PTR(matrix);
    CV_CHECK_PTR(matrix->data);

    matrix = CV_MATRIX4_FROM_IMAGE(image, matrix, index);
    CV_IMAGE_FREE(image);

    return matrix;
}

#pragma endregion Image

#pragma region Transform

Image *CV_RGB_TO_GRAY(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 3);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(1, src->h, src->w);

    CV_CHECK_IMAGE(dst);
    CV_CHECK_CHANNEL(dst, 1);

    for (int i = 0; i < src->h; i++)
    {
        for (int j = 0; j < src->w; j++)
        {
            float r = PIXEL(src, 0, i, j);
            float g = PIXEL(src, 1, i, j);
            float b = PIXEL(src, 2, i, j);

            float gray = (r + g + b) / 3.0;
            PIXEL(dst, 0, i, j) = NORM(gray);
        }
    }

    return dst;
}

Image *CV_GRAY_TO_RGB(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(3, src->h, src->w);

    CV_CHECK_IMAGE(dst);
    CV_CHECK_CHANNEL(dst, 3);

    for (int i = 0; i < src->h; i++)
    {
        for (int j = 0; j < src->w; j++)
        {
            float gray = PIXEL(src, 0, i, j);
            PIXEL(dst, 0, i, j) = gray;
            PIXEL(dst, 1, i, j) = gray;
            PIXEL(dst, 2, i, j) = gray;
        }
    }

    return dst;
}

Image *CV_APPLY_FILTER(Image *src, Image *dst, Matrix *kernel)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_COPY(src);

    CV_CHECK_PTR(kernel);
    CV_CHECK_PTR(kernel->data);

    int k = kernel->dim1 / 2;

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                float sum = 0;
                for (int m = 0; m < kernel->dim1; m++)
                {
                    for (int n = 0; n < kernel->dim2; n++)
                    {
                        int x = i + m - k;
                        int y = j + n - k;

                        if (x < 0 || x >= src->h || y < 0 || y >= src->w)
                            continue;

                        sum += PIXEL(src, c, x, y) * MATRIX(kernel, m, n);
                    }
                }

                PIXEL(dst, c, i, j) = NORM(sum);
            }
        }
    }

    return dst;
}

Matrix *CV_GET_GAUSSIAN_KERNEL(int size, float sigma)
{
    if (size % 2 == 0)
        errx(1, "Kernel size must be odd");

    Matrix *kernel = matrix_init(size, size, NULL);

    CV_CHECK_PTR(kernel);
    CV_CHECK_PTR(kernel->data);

    if (sigma == 0)
        sigma = (size - 1) / 2;

    int center = size / 2;
    float sum = 0.0;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            float x = i - center;
            float y = j - center;

            float value = exp(-(x * x + y * y) / (2 * sigma * sigma));
            MATRIX(kernel, i, j) = value;
            sum += value;
        }
    }

    // normalize kernel
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            MATRIX(kernel, i, j) /= sum;

    return kernel;
}

Image *CV_GAUSSIAN_BLUR(Image *src, Image *dst, int size, float sigma)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);

    Matrix *kernel = CV_GET_GAUSSIAN_KERNEL(size, sigma);
    dst = CV_APPLY_FILTER(src, dst, kernel);

    matrix_destroy(kernel);

    return dst;
}

Matrix *CV_GET_SHARPEN_KERNEL(float sigma)
{
    Matrix *kernel = matrix_init(3, 3, NULL);

    CV_CHECK_PTR(kernel);
    CV_CHECK_PTR(kernel->data);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == 1 && j == 1)
                MATRIX(kernel, i, j) = 4 * sigma;
            else if (i == 1 || j == 1)
                MATRIX(kernel, i, j) = -1 * sigma;
            else
                MATRIX(kernel, i, j) = 0;
        }
    }

    MATRIX(kernel, 1, 1) += 1;
    return kernel;
}

Image *CV_SHARPEN(Image *src, Image *dst, float sigma)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);

    Matrix *kernel = CV_GET_SHARPEN_KERNEL(sigma);

    dst = CV_APPLY_FILTER(src, dst, kernel);
    matrix_destroy(kernel);

    return dst;
}

Matrix *CV_GET_SOBEL_KERNEL_X()
{
    const float x[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    Matrix *kernel = matrix_init(3, 3, x);

    CV_CHECK_PTR(kernel);
    CV_CHECK_PTR(kernel->data);

    return kernel;
}

Matrix *CV_GET_SOBEL_KERNEL_Y()
{
    const float y[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    Matrix *kernel = matrix_init(3, 3, y);

    CV_CHECK_PTR(kernel);
    CV_CHECK_PTR(kernel->data);

    return kernel;
}

Image *CV_SOBEL_PROCESS(Image *src, Image *dst, Image *dst_x, Image *dst_y)
{
    CV_CHECK_IMAGE(src);

    // check if dst_x and dst_y are NULL
    int null_x = dst_x == NULL;
    int null_y = dst_y == NULL;

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);

    Matrix *kernel_x = CV_GET_SOBEL_KERNEL_X();
    Matrix *kernel_y = CV_GET_SOBEL_KERNEL_Y();

    if (null_x)
        dst_x = CV_APPLY_FILTER(src, NULL, kernel_x);

    if (null_y)
        dst_y = CV_APPLY_FILTER(src, NULL, kernel_y);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                float x = PIXEL(dst_x, c, i, j);
                float y = PIXEL(dst_y, c, i, j);

                PIXEL(dst, c, i, j) = NORM(sqrt(x * x + y * y));
            }
        }
    }

    matrix_destroy(kernel_x);
    matrix_destroy(kernel_y);

    if (null_x)
        CV_IMAGE_FREE(dst_x);

    if (null_y)
        CV_IMAGE_FREE(dst_y);

    return dst;
}

Image *CV_SOBEL(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);

    dst = CV_SOBEL_PROCESS(src, dst, NULL, NULL);

    return dst;
}

float CV_ARCTAN2(Image *src, int c, int i, int j)
{
    float x = PIXEL(src, c, i, j);
    float y = PIXEL(src, c, i + 1, j + 1);

    return atan2(y, x);
}

Image *CV_NON_MAX_SUPPRESSION(Image *src, Image *dst, Image *dst_x, Image *dst_y)
{
    CV_CHECK_IMAGE(src);

    // check if dst_x and dst_y are NULL
    int null_x = dst_x == NULL;
    int null_y = dst_y == NULL;

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);

    if (null_x)
        dst_x = CV_SOBEL_PROCESS(src, NULL, NULL, NULL);

    if (null_y)
        dst_y = CV_SOBEL_PROCESS(src, NULL, NULL, NULL);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                float angle = CV_ARCTAN2(dst_y, c, i, j);
                float mag = PIXEL(src, c, i, j);

                if (angle < 0)
                    angle += 2 * PI;

                if (angle >= 0 && angle < PI8)
                {
                    if (mag < PIXEL(src, c, i, j + 1) || mag < PIXEL(src, c, i, j - 1))
                        PIXEL(dst, c, i, j) = 0;
                    else
                        PIXEL(dst, c, i, j) = mag;
                }
                else if (angle >= PI8 && angle < 3 * PI8)
                {
                    if (mag < PIXEL(src, c, i + 1, j + 1) || mag < PIXEL(src, c, i - 1, j - 1))
                        PIXEL(dst, c, i, j) = 0;
                    else
                        PIXEL(dst, c, i, j) = mag;
                }
                else if (angle >= 3 * PI8 && angle < 5 * PI8)
                {
                    if (mag < PIXEL(src, c, i + 1, j) || mag < PIXEL(src, c, i - 1, j))
                        PIXEL(dst, c, i, j) = 0;
                    else
                        PIXEL(dst, c, i, j) = mag;
                }
                else if (angle >= 5 * PI8 && angle < 7 * PI8)
                {
                    if (mag < PIXEL(src, c, i + 1, j - 1) || mag < PIXEL(src, c, i - 1, j + 1))
                        PIXEL(dst, c, i, j) = 0;
                    else
                        PIXEL(dst, c, i, j) = mag;
                }
                else if (angle >= 7 * PI8 && angle <= 2 * PI)
                {
                    if (mag < PIXEL(src, c, i, j + 1) || mag < PIXEL(src, c, i, j - 1))
                        PIXEL(dst, c, i, j) = 0;
                    else
                        PIXEL(dst, c, i, j) = mag;
                }
            }
        }
    }

    if (null_x)
        CV_IMAGE_FREE(dst_x);

    if (null_y)
        CV_IMAGE_FREE(dst_y);

    return dst;
}

Image *CV_HYSTERESIS_THRESHOLDING(Image *src, Image *dst, float low, float high)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);

    CV_CHECK_CHANNEL(src, 1);
    CV_CHECK_CHANNEL(dst, 1);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 1; i < src->h - 1; i++)
        {
            for (int j = 1; j < src->w - 1; j++)
            {
                float p = PIXEL(src, c, i, j);

                if (p > high)
                    PIXEL(dst, c, i, j) = 1;
                else if (p < low)
                    PIXEL(dst, c, i, j) = 0;
                else
                {
                    float p1 = PIXEL(src, c, i - 1, j);
                    float p2 = PIXEL(src, c, i + 1, j);
                    float p3 = PIXEL(src, c, i, j - 1);
                    float p4 = PIXEL(src, c, i, j + 1);

                    if (p1 > high || p2 > high || p3 > high || p4 > high)
                        PIXEL(dst, c, i, j) = 1;
                    else
                        PIXEL(dst, c, i, j) = 0;
                }
            }
        }
    }

    return dst;
}

Image *CV_CANNY(Image *src, Image *dst, float low, float high)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);
    CV_CHECK_IMAGE(dst);

    CV_CHECK_CHANNEL(src, 1);
    CV_CHECK_CHANNEL(dst, 1);

    Matrix *kernel_x = CV_GET_SOBEL_KERNEL_X();
    Matrix *kernel_y = CV_GET_SOBEL_KERNEL_Y();

    Image *dst_x = CV_APPLY_FILTER(src, NULL, kernel_x);
    Image *dst_y = CV_APPLY_FILTER(src, NULL, kernel_y);

    CV_SOBEL_PROCESS(src, dst, dst_x, dst_y);

    Image *non_max = CV_NON_MAX_SUPPRESSION(dst, NULL, dst_x, dst_y);
    dst = CV_HYSTERESIS_THRESHOLDING(non_max, dst, low, high);

    matrix_destroy(kernel_x);
    matrix_destroy(kernel_y);
    CV_IMAGE_FREE(dst_x);
    CV_IMAGE_FREE(dst_y);
    CV_IMAGE_FREE(non_max);

    return dst;
}

Image *CV_OTSU(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
        dst = CV_IMAGE_COPY(src);
    CV_CHECK_IMAGE(dst);

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
        float µ1 = sumB / q1;
        float µ2 = (sum - sumB) / q2;

        float σ2b = q1 * q2 * ((µ1 - µ2) * (µ1 - µ2));

        if (σ2b > var_max)
        {
            var_max = σ2b;
            threshold = t;
        }
    }

    for (int i = 0; i < N; i++)
    {
        float p = src->data[i] * 255.0;
        if (p > threshold)
            dst->data[i] = 0;
        else
            dst->data[i] = 1;
    }

    return dst;
}

Image *CV_OR(Image *src1, Image *src2, Image *dst)
{
    CV_CHECK_IMAGE(src1);
    CV_CHECK_IMAGE(src2);
    CV_CHECK_CHANNEL(src1, 1);
    CV_CHECK_CHANNEL(src2, 1);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src1->c, src1->h, src1->w);
    CV_CHECK_IMAGE(dst);

    int N = src1->h * src1->w;

    for (int i = 0; i < N; i++)
    {
        if (src1->data[i] == 1 || src2->data[i] == 1)
            dst->data[i] = 1;
        else
            dst->data[i] = 0;
    }

    return dst;
}

#pragma endregion Transform