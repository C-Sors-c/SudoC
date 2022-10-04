
#include "../include/layer.h"


#pragma region fully_connected_layer

// fully connected layer
struct FCLayer {
    int input_size;
    int output_size;
    float (*activation_func)(float);
    Matrix *weights;
    Matrix *biases;
    Matrix *activations;
    Matrix *deltas;
    Matrix *weights_gradient;
    Matrix *biases_gradient;
};


typedef struct FCLayer FCLayer;


// initialize a weight matrix
Matrix *fc_weight_init(int dim1, int dim2)
{
    Matrix *weight = matrix_init(dim1, dim2, NULL);
    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim2; j++) {
            matrix_set(weight, i, j, (float)rand() / (float)(RAND_MAX / 2) - 1);
        }
    }
    return weight;
}

// initialize a bias matrix
Matrix *fc_bias_init(int dim1, int dim2)
{
    Matrix *bias = matrix_init(dim1, dim2, NULL);
    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim2; j++) {
            matrix_set(bias, i, j, 0);
        }
    }
    return bias;
}


// create a new fully connected layer
FCLayer *fc_layer_init(int input_size, int output_size, float (*activation_func)(float))
{
    FCLayer *layer = malloc(sizeof(FCLayer));
    layer->input_size = input_size;
    layer->output_size = output_size;
    layer->activation_func = activation_func;
    
    layer->weights = fc_weight_init(output_size, input_size);
    layer->biases = fc_bias_init(output_size, 1);

    layer->activations = matrix_init(output_size, 1, NULL);
    layer->deltas = matrix_init(output_size, 1, NULL);
    layer->weights_gradient = matrix_init(output_size, input_size, NULL);
    layer->biases_gradient = matrix_init(output_size, 1, NULL);
    return layer;
}

#pragma endregion fully_connected_layer

#pragma region convolutional_layer

// convolutional layer
struct ConvLayer {
    int input_size;
    int output_size;
    int kernel_size;
    int stride;
    int padding;
    float (*activation_func)(float);
    Matrix4 *weights;
    Matrix4 *biases;
    Matrix4 *activations;
    Matrix4 *deltas;
    Matrix4 *weights_gradient;
    Matrix4 *biases_gradient;
};
typedef struct ConvLayer ConvLayer;

Matrix4 *conv_weight_init(int dim1, int dim2, int dim3, int dim4)
{
    Matrix4 *weight = matrix4_init(dim1, dim2, dim3, dim4, NULL);
    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim2; j++) {
            for (int k = 0; k < dim3; k++) {
                for (int l = 0; l < dim4; l++) {
                    matrix4_set(weight, i, j, k, l, (float)rand() / (float)(RAND_MAX / 2) - 1);
                }
            }
        }
    }
    return weight;
}

// TODO: initialize a bias matrix & create a new convolutional layer


#pragma endregion convolutional_layer