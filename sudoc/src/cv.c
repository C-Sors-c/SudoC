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

Image *CV_IMAGE_COPY_PART(Image *image, int xstart, int ystart, int xend, int yend)
{
    CV_CHECK_IMAGE(image);
    Image *copy = CV_IMAGE_INIT(image->c, yend - ystart, xend - xstart);

    for (int c = 0; c < image->c; c++)
    {
        for (int i = ystart; i < yend; i++)
        {
            for (int j = xstart; j < xend; j++)
            {
                int index = c * image->h * image->w + i * image->w + j;
                int index_copy = c * copy->h * copy->w + (i - ystart) * copy->w + (j - xstart);
                PIXEL(copy, c, i - ystart, j - xstart) = PIXEL(image, c, i, j);
            }
        }
    }

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

    if (mode == RGB)
        return image;

    if (mode == GRAYSCALE)
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
                MATRIX(kernel, i, j) = 4.0 * sigma;
            else if (i == 1 || j == 1)
                MATRIX(kernel, i, j) = -1.0 * sigma;
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
    float x[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    Matrix *kernel = matrix_init(3, 3, x);

    CV_CHECK_PTR(kernel);
    CV_CHECK_PTR(kernel->data);

    return kernel;
}

Matrix *CV_GET_SOBEL_KERNEL_Y()
{
    float y[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
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

float CV_THRESHOLD(Image *src)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

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

Image *CV_OTSU(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
        dst = CV_IMAGE_COPY(src);
    CV_CHECK_IMAGE(dst);

    float threshold = CV_THRESHOLD(src);

    for (int h = 0; h < src->h; h++)
    {
        for (int w = 0; w < src->w; w++)
        {
            float p = PIXEL(src, 0, h, w);
            if (p > threshold)
                PIXEL(dst, 0, h, w) = 0;
            else
                PIXEL(dst, 0, h, w) = 1;
        }
    }

    return dst;
}

Image *CV_ADAPTIVE_THRESHOLD(Image *src, Image *dst, int block_size, float otsu_weight, float c)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
        dst = CV_IMAGE_COPY(src);
    CV_CHECK_IMAGE(dst);

    if (block_size % 2 == 0)
        block_size++;

    if (otsu_weight < 0)
        otsu_weight = 0;
    else if (otsu_weight > 1)
        otsu_weight = 1;

    float otsu = CV_THRESHOLD(src);

    float gaussian_weight = 1.0 - otsu_weight;

    Matrix *kernel = CV_GET_GAUSSIAN_KERNEL(block_size, 1);

    Image *mean = CV_APPLY_FILTER(src, NULL, kernel);
    Image *var = CV_APPLY_FILTER(src, NULL, kernel);

    for (int h = 0; h < src->h; h++)
    {
        for (int w = 0; w < src->w; w++)
        {
            float p = PIXEL(src, 0, h, w);
            float m = PIXEL(mean, 0, h, w);
            float v = PIXEL(var, 0, h, w);

            // float threshold = m + c * sqrt(v - m * m);
            float local_threshold = otsu_weight * otsu + gaussian_weight * (m - c * sqrt(v));
            float threshold = (local_threshold * gaussian_weight) + (otsu * otsu_weight);

            if (p > threshold)
                PIXEL(dst, 0, h, w) = 0;
            else
                PIXEL(dst, 0, h, w) = 1;
        }
    }

    matrix_destroy(kernel);
    CV_IMAGE_FREE(mean);
    CV_IMAGE_FREE(var);

    return dst;
}

Image *CV_OR(Image *src1, Image *src2, Image *dst)
{
    CV_CHECK_IMAGE(src1);
    CV_CHECK_IMAGE(src2);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src1->c, src1->h, src1->w);
    CV_CHECK_IMAGE(dst);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(src1, c, i, j);
                float p2 = PIXEL(src2, c, i, j);

                if (p1 == 1 || p2 == 1)
                    PIXEL(dst, c, i, j) = 1;
                else
                    PIXEL(dst, c, i, j) = 0;
            }
        }
    }

    return dst;
}

Image *CV_AND(Image *src1, Image *src2, Image *dst)
{
    CV_CHECK_IMAGE(src1);
    CV_CHECK_IMAGE(src2);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src1->c, src1->h, src1->w);
    CV_CHECK_IMAGE(dst);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(src1, c, i, j);
                float p2 = PIXEL(src2, c, i, j);

                if (p1 == 1 && p2 == 1)
                    PIXEL(dst, c, i, j) = 1;
                else
                    PIXEL(dst, c, i, j) = 0;
            }
        }
    }
    return dst;
}

Image *CV_XOR(Image *src1, Image *src2, Image *dst)
{
    CV_CHECK_IMAGE(src1);
    CV_CHECK_IMAGE(src2);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src1->c, src1->h, src1->w);
    CV_CHECK_IMAGE(dst);

    for (int c = 0; c < src1->c; c++)
    {
        for (int i = 0; i < src1->h; i++)
        {
            for (int j = 0; j < src1->w; j++)
            {
                float p1 = PIXEL(src1, c, i, j);
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

    return dst;
}

Image *CV_NOT(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
        dst = CV_IMAGE_INIT(src->c, src->h, src->w);
    CV_CHECK_IMAGE(dst);

    for (int c = 0; c < src->c; c++)
    {
        for (int i = 0; i < src->h; i++)
        {
            for (int j = 0; j < src->w; j++)
            {
                float p = PIXEL(src, c, i, j);

                if (p == 1)
                    PIXEL(dst, c, i, j) = 0;
                else
                    PIXEL(dst, c, i, j) = 1;
            }
        }
    }

    return dst;
}

Image *CV_DILATION(Image *src, Image *dst, int k)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
        dst = CV_IMAGE_COPY(src);
    CV_CHECK_IMAGE(dst);

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

                    float p = PIXEL(src, 0, h + i, w + j);

                    if (p > max)
                        max = p;
                }
            }

            PIXEL(dst, 0, h, w) = max;
        }
    }

    return dst;
}

Image *CV_EROSION(Image *src, Image *dst, int k)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
        dst = CV_IMAGE_COPY(src);
    CV_CHECK_IMAGE(dst);

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

                    float p = PIXEL(src, 0, h + i, w + j);

                    if (p < min)
                        min = p;
                }
            }

            PIXEL(dst, 0, h, w) = min;
        }
    }

    return dst;
}

Uint32 CV_RGB(Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 color = 0;
    return color | r << 16 | g << 8 | b;
}

Image *CV_DRAW_POINT(Image *dst, int x, int y, int width, Uint32 color)
{
    CV_CHECK_IMAGE(dst);

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

                int r = (color >> (16 - c * 8)) & 0xff;
                PIXEL(dst, c, y + j, x + i) = r / 255.0;
            }
        }
    }

    return dst;
}

Image *CV_DRAW_LINE(Image *dst, int x1, int y1, int x2, int y2, int width, Uint32 color)
{
    CV_CHECK_IMAGE(dst);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        for (int i = 0; i < width; i++)
            for (int j = 0; j < width; j++)
                CV_DRAW_POINT(dst, x1 + i, y1 + j, width, color);

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

Image *CV_DRAW_RECT(Image *dst, int x, int y, int w, int h, int width, Uint32 color)
{
    CV_CHECK_IMAGE(dst);

    CV_DRAW_LINE(dst, x, y, x + w, y, width, color);
    CV_DRAW_LINE(dst, x + w, y, x + w, y + h, width, color);
    CV_DRAW_LINE(dst, x + w, y + h, x, y + h, width, color);
    CV_DRAW_LINE(dst, x, y + h, x, y, width, color);

    return dst;
}

Image *CV_DRAW_CIRCLE(Image *dst, int x, int y, int r, int width, Uint32 color)
{
    CV_CHECK_IMAGE(dst);

    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int _x = 0;
    int _y = r;

    CV_DRAW_POINT(dst, x, y + r, width, color);
    CV_DRAW_POINT(dst, x, y - r, width, color);
    CV_DRAW_POINT(dst, x + r, y, width, color);
    CV_DRAW_POINT(dst, x - r, y, width, color);

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

        CV_DRAW_POINT(dst, x - _x, y + _y, width, color);
        CV_DRAW_POINT(dst, x + _x, y + _y, width, color);
        CV_DRAW_POINT(dst, x + _x, y - _y, width, color);
        CV_DRAW_POINT(dst, x - _x, y - _y, width, color);
        CV_DRAW_POINT(dst, x + _y, y + _x, width, color);
        CV_DRAW_POINT(dst, x - _y, y + _x, width, color);
        CV_DRAW_POINT(dst, x + _y, y - _x, width, color);
        CV_DRAW_POINT(dst, x - _y, y - _x, width, color);
    }

    return dst;
}

Image *CV_DRAW_DIGIT(Image *dst, int x, int y, int digit, int width, Uint32 color)
{
    CV_CHECK_IMAGE(dst);

    if (digit < 0 || digit > 9)
    {
        ERROR_INFO;
        errx(1, "CV_DRAW_DIGIT: digit must be between 0 and 9");
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
                CV_DRAW_LINE(dst, x, y, x + width, y, bar_w, color);
                break;
            case 1:
                CV_DRAW_LINE(dst, x + width, y, x + width, y + width, bar_w, color);
                break;
            case 2:
                CV_DRAW_LINE(dst, x + width, y + width, x + width, y + width * 2, bar_w, color);
                break;
            case 3:
                CV_DRAW_LINE(dst, x, y + width * 2, x + width, y + width * 2, bar_w, color);
                break;
            case 4:
                CV_DRAW_LINE(dst, x, y + width, x, y + width * 2, bar_w, color);
                break;
            case 5:
                CV_DRAW_LINE(dst, x, y, x, y + width, bar_w, color);
                break;
            case 6:
                CV_DRAW_LINE(dst, x, y + width, x + width, y + width, bar_w, color);
                break;
            }
        }
    }

    return dst;
}

int CV_FLOOR(float x)
{
    if (x >= 0)
        return (int)x;
    else
        return (int)x - 1;
}

int CV_ROUND(float x)
{
    if (x >= 0)
        return (int)(x + 0.5);
    else
        return (int)(x - 0.5);
}

int CV_CEIL(float x)
{
    if (x == (int)x)
        return (int)x;
    else
        return (int)x + 1;
}

int *CV_HOUGH_LINES(Image *src, int threshold, int *nlines)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    int w = src->w;
    int h = src->h;

    int *accumulator = (int *)malloc(sizeof(int) * w * h * 180);
    memset(accumulator, 0, sizeof(int) * w * h * 180);

    int max = 0;
    // int max_theta = 0;
    // int max_rho = 0;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (PIXEL(src, 0, y, x) == 0)
                continue;

            for (int theta = 0; theta < 180; theta++)
            {
                float tf = (float)theta * PI / 180.0;       // theta in radian
                int rho = (int)(x * cos(tf) + y * sin(tf)); // rho in pixel
                if (rho < 0)
                    rho = -rho;

                accumulator[rho * 180 + theta]++; // accumulate the votes for each rho and theta

                if (accumulator[rho * 180 + theta] > max)
                {
                    max = accumulator[rho * 180 + theta];
                    // max_theta = theta;
                    // max_rho = rho;
                }
            }
        }
    }

    // we count the number of lines to allocate the memory
    int nb_lines = 0;
    for (int i = 0; i < w * h * 180; i++)
        if (accumulator[i] >= threshold)
            nb_lines++;

    // we 2 integers per line (rho and theta)
    int *lines = (int *)malloc(sizeof(int) * nb_lines * 2);
    memset(lines, 0, sizeof(int) * nb_lines * 2);

    // we fill the lines array
    int j = 0;
    for (int i = 0; i < w * h * 180; i++)
    {
        if (accumulator[i] >= threshold)
        {
            lines[j * 2] = i / 180;     // rho
            lines[j * 2 + 1] = i % 180; // theta
            j++;
        }
    }

    // cleanup and return
    *nlines = nb_lines;
    free(accumulator);
    return lines;
}
int *CV_SIMPLIFY_HOUGH_LINES(int *lines, int nlines, int threshold, int *nsimplified)
{
    int *simplified = (int *)malloc(sizeof(int) * nlines * 2);
    memset(simplified, 0, sizeof(int) * nlines * 2);

    int j = 0;
    for (int i = 0; i < nlines; i++)
    {
        int rho = lines[i * 2];
        int theta = lines[i * 2 + 1];

        int found = 0;
        for (int k = 0; k < j; k++)
        {
            int rho2 = simplified[k * 2];
            int theta2 = simplified[k * 2 + 1];

            if (abs(rho - rho2) < threshold && abs(theta - theta2) < threshold)
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
            simplified[j * 2] = rho;
            simplified[j * 2 + 1] = theta;
            j++;
        }
    }

    *nsimplified = j;

    return simplified;
}

int *CV_REMOVE_DIAGONALS(int *lines, int nlines, int *nsimplified)
{
    int *filtered = (int *)malloc(sizeof(int) * nlines * 2);
    memset(filtered, 0, sizeof(int) * nlines * 2);

    int j = 0;
    for (int i = 0; i < nlines; i++)
    {
        int rho = lines[i * 2];
        int theta = lines[i * 2 + 1];

        if ((23 < theta && theta < 67) || (113 < theta && theta < 157))
            continue;
        else
        {
            filtered[j * 2] = rho;
            filtered[j * 2 + 1] = theta;
            j++;
        }
    }

    *nsimplified = j;

    return filtered;
}

Image *CV_DRAW_HOUGH_LINES(Image *dst, int *lines, int nlines, int weight, Uint32 color)
{
    CV_CHECK_IMAGE(dst);

    int w = dst->w;
    int h = dst->h;

    for (int i = 0; i < nlines; i++)
    {
        int rho = lines[i * 2];
        int theta = lines[i * 2 + 1];

        if (theta == 0)
        {
            CV_DRAW_LINE(dst, rho, 0, rho, h, weight, color);
        }
        else
        {
            float a = cos(theta * PI / 180.0);
            float b = sin(theta * PI / 180.0);
            float x0 = a * rho;
            float y0 = b * rho;
            float x1 = x0 + w * (-b);
            float y1 = y0 + w * a;
            float x2 = x0 - w * (-b);
            float y2 = y0 - w * a;

            int x1i = (int)x1;
            int y1i = (int)y1;
            int x2i = (int)x2;
            int y2i = (int)y2;

            CV_DRAW_LINE(dst, x1i, y1i, x2i, y2i, weight, color);
        }
    }

    return dst;
}

float CV_HOUGH_LINES_ORIENTATION(int *lines, int nlines)
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

    free(histo);

    return max_theta;
}

int *CV_FIND_LARGEST_CONTOUR(Image *src, int *nrects)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    Image *p = CV_IMAGE_COPY(src);

    int w = p->w;
    int h = p->h;

    int *contours = (int *)malloc(sizeof(int) * w * h * 2);
    memset(contours, 0, sizeof(int) * w * h * 2);

    int *visited = (int *)malloc(sizeof(int) * w * h * 2);
    memset(visited, 0, sizeof(int) * w * h * 2);

    int *max_contours;

    int ncontours = 0;
    int max_contour = 0;

    int min_x = 0;
    int min_y = 0;
    int max_x = 0;
    int max_y = 0;

    int P1_x, P1_y, P2_x, P2_y, P3_x, P3_y, P4_x, P4_y = 0;

    int max_area = 0;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (PIXEL(p, 0, y, x) == 0)
                continue;

            if (visited[y * w + x])
                continue;

            int *stack = (int *)malloc(sizeof(int) * w * h * 2);
            memset(stack, 0, sizeof(int) * w * h * 2);

            int nstack = 0;

            stack[nstack * 2] = x;
            stack[nstack * 2 + 1] = y;
            nstack++;

            int xmin = x;
            int xmax = x;
            int ymin = y;
            int ymax = y;

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

                if (x < xmin)
                    xmin = x;
                if (x > xmax)
                    xmax = x;
                if (y < ymin)
                    ymin = y;
                if (y > ymax)
                    ymax = y;

                if (x > 0 && PIXEL(p, 0, y, x - 1) == 1 && !visited[y * w + x - 1])
                {
                    stack[nstack * 2] = x - 1;
                    stack[nstack * 2 + 1] = y;
                    nstack++;
                }

                if (x < w - 1 && PIXEL(p, 0, y, x + 1) == 1 && !visited[y * w + x + 1])
                {
                    stack[nstack * 2] = x + 1;
                    stack[nstack * 2 + 1] = y;
                    nstack++;
                }

                if (y > 0 && PIXEL(p, 0, y - 1, x) == 1 && !visited[(y - 1) * w + x])
                {
                    stack[nstack * 2] = x;
                    stack[nstack * 2 + 1] = y - 1;
                    nstack++;
                }

                if (y < h - 1 && PIXEL(p, 0, y + 1, x) == 1 && !visited[(y + 1) * w + x])
                {
                    stack[nstack * 2] = x;
                    stack[nstack * 2 + 1] = y + 1;
                    nstack++;
                }
            }

            free(stack);

            int p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y = 0;

            for (int i = 0; i < ncontours; i++)
            {
                int x = contours[i * 2];
                int y = contours[i * 2 + 1];

                // P1
                if (x == xmin)
                {
                    p1_x = x;
                    p1_y = y;
                }

                // P2
                else if (y == ymin)
                {
                    p2_x = x;
                    p2_y = y;
                }

                // P3
                else if (x == xmax)
                {
                    p3_x = x;
                    p3_y = y;
                }

                // P4
                else if (y == ymax)
                {
                    p4_x = x;
                    p4_y = y;
                }
            }

            // compute area from unsorted points
            int width = (p3_x - p1_x);
            int height = (p4_y - p2_y);
            int area = abs(width * height);

            if (ncontours > max_contour && area > max_area)
            {
                max_contour = ncontours;
                max_area = area;

                min_x = xmin;
                min_y = ymin;
                max_x = xmax;
                max_y = ymax;

                P1_x = p1_x;
                P1_y = p1_y;
                P2_x = p2_x;
                P2_y = p2_y;
                P3_x = p3_x;
                P3_y = p3_y;
                P4_x = p4_x;
                P4_y = p4_y;

                max_contours = (int *)malloc(sizeof(int) * max_contour * 2);
                memcpy(max_contours, contours, sizeof(int) * max_contour * 2);
            }

            ncontours = 0;
        }
    }

    free(visited);
    free(contours);

    CV_IMAGE_FREE(p);

    printf("min_x: %d, min_y: %d, max_x: %d, max_y: %d\n", min_x, min_y, max_x, max_y);

    printf("P1(%d, %d)\n", P1_x, P1_y);
    printf("P2(%d, %d)\n", P2_x, P2_y);
    printf("P3(%d, %d)\n", P3_x, P3_y);
    printf("P4(%d, %d)\n", P4_x, P4_y);

    printf("max_contour: %d, max_area: %d\n", max_contour, max_area);

    *nrects = 4;
    int *rects = (int *)malloc(sizeof(int) * 8);
    rects[0] = P1_x;
    rects[1] = P1_y;
    rects[2] = P2_x;
    rects[3] = P2_y;
    rects[4] = P3_x;
    rects[5] = P3_y;
    rects[6] = P4_x;
    rects[7] = P4_y;

    return rects;
}

int *CV_GRID_INTERSECTION(Image *src, int *lines, int nlines, int *nintersection)
{
    CV_CHECK_IMAGE(src);

    int w = src->w;
    int h = src->h;

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

            if (x < 0 || x > w || y < 0 || y > h)
                continue;

            intersection[j * 2] = (int)x;
            intersection[j * 2 + 1] = (int)y;
            j++;
        }
    }

    *nintersection = j;

    return intersection;
}
// sort intersections by x value
int *CV_SORT_INTERSECTIONS(int *intersections, int nintersections)
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
            int x2 = sorted[j * 2];
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

Image *CV_ROTATE(Image *src, Image *dst, float angle, Uint32 background)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
    {
        dst = CV_ZEROS(src->c, src->h, src->w);
    }

    float radians = angle * PI / 180.0;
    int hwidth = src->w / 2;
    int hheight = src->h / 2;
    double sinma = sin(-radians);
    double cosma = cos(-radians);

    for (int x = 0; x < src->w; x++)
    {
        for (int y = 0; y < src->h; y++)
        {

            int xt = x - hwidth;
            int yt = y - hheight;

            int xs = (int)round((cosma * xt - sinma * yt) + hwidth);
            int ys = (int)round((sinma * xt + cosma * yt) + hheight);

            if (xs >= 0 && xs < src->w && ys >= 0 && ys < src->h)
            {
                for (int c = 0; c < src->c; c++)
                {
                    PIXEL(dst, c, y, x) = PIXEL(src, c, ys, xs);
                }
            }
            else
            {
                for (int c = 0; c < src->c; c++)
                {
                    int r = (background >> (16 - c * 8)) & 0xff;
                    PIXEL(dst, c, y, x) = r / 255.0;
                }
            }
        }
    }
    return dst;
}

Image *CV_RESIZE(Image *src, Image *dst, float scale)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
    {
        dst = CV_ZEROS(src->c, src->h * scale, src->w * scale);
    }

    for (int x = 0; x < dst->w; x++)
    {
        for (int y = 0; y < dst->h; y++)
        {
            int xs = (int)(x / scale);
            int ys = (int)(y / scale);

            if (xs >= 0 && xs < src->w && ys >= 0 && ys < src->h)
            {
                for (int c = 0; c < src->c; c++)
                {
                    PIXEL(dst, c, y, x) = PIXEL(src, c, ys, xs);
                }
            }
            else
            {
                for (int c = 0; c < src->c; c++)
                {
                    PIXEL(dst, c, y, x) = 0;
                }
            }
        }
    }
    return dst;
}

Image *CV_ZOOM(Image *src, Image *dst, float scale, Uint32 background)
{
    CV_CHECK_IMAGE(src);

    if (dst == NULL)
    {
        dst = CV_ZEROS(src->c, src->h, src->w);
    }

    // zoom and keeps previous dimensions

    int hwidth = src->w / 2;
    int hheight = src->h / 2;
    int hwidth2 = dst->w / 2;
    int hheight2 = dst->h / 2;

    for (int x = 0; x < dst->w; x++)
    {
        for (int y = 0; y < dst->h; y++)
        {
            int xs = (int)((x - hwidth2) / scale) + hwidth;
            int ys = (int)((y - hheight2) / scale) + hheight;

            if (xs >= 0 && xs < src->w && ys >= 0 && ys < src->h)
            {
                for (int c = 0; c < src->c; c++)
                {
                    PIXEL(dst, c, y, x) = PIXEL(src, c, ys, xs);
                }
            }
            else
            {
                for (int c = 0; c < src->c; c++)
                {
                    int r = (background >> (16 - c * 8)) & 0xff;
                    PIXEL(dst, c, y, x) = r / 255.0;
                }
            }
        }
    }
    return dst;
}

#pragma endregion Transform
