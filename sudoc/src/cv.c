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
    memset(image->data, 255, channels * height * width * sizeof(pixel_t));

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
            pixel_t r, g, b;
            SDL_GetRGB(pixel, format, &r, &g, &b);
            PIXEL(image, 0, i, j) = r;
            PIXEL(image, 1, i, j) = g;
            PIXEL(image, 2, i, j) = b;
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
                pixel_t r = PIXEL(image, 0, i, j);
                pixel_t g = PIXEL(image, 1, i, j);
                pixel_t b = PIXEL(image, 2, i, j);
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
                pixel_t r = PIXEL(image, 0, i, j);
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

    for (int c = 0; c < image->c; c++)
    {
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                float value = PIXEL(image, c, i, j) / 255.0;
                m4_set(matrix, index, c, i, j, value);
            }
        }
    }
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

    for (int c = 0; c < matrix->dim2; c++)
    {
        for (int i = 0; i < matrix->dim3; i++)
        {
            for (int j = 0; j < matrix->dim4; j++)
            {
                float value = m4_get(matrix, index, c, i, j);
                PIXEL(image, c, i, j) = value * 255;
            }
        }
    }
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
    {
        dst = CV_IMAGE_INIT(1, src->h, src->w);
    }

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
            PIXEL(dst, 0, i, j) = gray;
        }
    }

    return dst;
}

Image *CV_GRAY_TO_RGB(Image *src, Image *dst)
{
    CV_CHECK_IMAGE(src);
    CV_CHECK_CHANNEL(src, 1);

    if (dst == NULL)
    {
        dst = CV_IMAGE_INIT(3, src->h, src->w);
    }

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

Matrix *CV_GET_GAUSSIAN_KERNEL(int size, float sigma)
{
    Matrix *kernel = matrix_init(size, size, NULL);
    CV_CHECK_PTR(kernel);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            float x = j - size / 2.0;
            float y = i - size / 2.0;

            float value = exp(-(x * x + y * y) / (2.0 * sigma * sigma));
            MATRIX(kernel, i, j) = value / (2.0 * PI * sigma * sigma);
        }
    }

    return kernel;
}

#pragma endregion Transform