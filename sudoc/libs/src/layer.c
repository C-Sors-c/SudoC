
#include "layer.h"

struct Layer {
    int input_size;
    int output_size;
    Matrix *weights;
    Matrix *biases;
    Matrix *activations;
    Matrix *deltas;
    Matrix *weights_gradient;
    Matrix *biases_gradient;
};

typedef struct Layer Layer;


// TODO: implement a kernel struct
// struct Kernel {
// };
// typedef struct Kernel Kernel;


Matrix *weight_init(int dim1, int dim2)
{
    Matrix *weight = matrix_init(dim1, dim2, NULL);
    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim2; j++) {
            matrix_set(weight, i, j, (float)rand() / (float)(RAND_MAX / 2) - 1);
        }
    }
    return weight;
}

Layer *layer_init(int input_size, int output_size)
{
    Layer *layer = malloc(sizeof(Layer));
    layer->input_size = input_size;
    layer->output_size = output_size;

    layer->weights = matrix_init(output_size, input_size, NULL);
    layer->biases = matrix_init(output_size, 1, NULL);
    layer->activations = matrix_init(output_size, 1, NULL);
    layer->deltas = matrix_init(output_size, 1, NULL);
    layer->weights_gradient = matrix_init(output_size, input_size, NULL);
    layer->biases_gradient = matrix_init(output_size, 1, NULL);
    return layer;
}


