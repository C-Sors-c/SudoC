
#include "../include/layer.h"

#pragma region fully_connected_layer

// fully connected layer
struct FCLayer
{
    int input_size;
    int output_size;
    float (*activation_func)(float);
    float (*d_activation_func)(float);
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
    for (int i = 0; i < dim1; i++)
        for (int j = 0; j < dim2; j++)
            weight->data[i][j] = (float)rand() / (float)(RAND_MAX / 2) - 1;
    return weight;
}

// initialize a bias matrix
Matrix *fc_bias_init(int dim1, int dim2)
{
    Matrix *bias = matrix_init(dim1, dim2, NULL);
    for (int i = 0; i < dim1; i++)
        for (int j = 0; j < dim2; j++)
            bias->data[i][j] = 0;
    return bias;
}

// create a new fully connected layer
FCLayer *fc_layer_init(int input_size, int output_size, int batch_size, float (*activation_func)(float), bool load_weights)
{
    FCLayer *layer = malloc(sizeof(FCLayer));
    layer->input_size = input_size;
    layer->output_size = output_size;
    layer->activation_func = activation_func;

    // initialize weights and biases
    if (load_weights)
    {
        // parser for weights
    }
    else
    {
        layer->weights = fc_weight_init(output_size, input_size);
        layer->biases = fc_bias_init(output_size, 1);
    }

    // initialize activations and deltas
    layer->activations = matrix_init(output_size, batch_size, NULL);
    layer->deltas = matrix_init(output_size, batch_size, NULL);

    // initialize gradients
    layer->weights_gradient = matrix_init(output_size, input_size, NULL);
    layer->biases_gradient = matrix_init(output_size, 1, NULL);

    return layer;
}

// forward pass for an input of shape: (batch_size, input_size)
void fc_layer_forward(FCLayer *layer, Matrix *input)
{
    // calculate activations
    matrix_multiply(layer->weights, input, layer->activations);
    matrix_add(layer->activations, layer->biases, layer->activations);
    matrix_map_function(layer->activations, layer->activation_func);
}

// backward pass
void fc_layer_backward(FCLayer *layer, Matrix *previous_layer_deltas, Matrix *previous_layer_activations)
{
    // calculate deltas
    matrix_multiply(matrix_transpose(layer->weights), previous_layer_deltas, layer->deltas);
    matrix_map_function(layer->deltas, layer->d_activation_func);

    // calculate gradients
    matrix_multiply(previous_layer_deltas, matrix_transpose(previous_layer_activations), layer->weights_gradient);
    matrix_copy(previous_layer_deltas, layer->biases_gradient);

    // update weights and biases
    matrix_scalar_multiply(layer->weights_gradient, -1);
    matrix_scalar_multiply(layer->biases_gradient, -1);

    matrix_add(layer->weights, layer->weights_gradient, layer->weights);
    matrix_add(layer->biases, layer->biases_gradient, layer->biases);

    // reset gradients
    matrix_zero(layer->weights_gradient);
    matrix_zero(layer->biases_gradient);
}

// destroy a fully connected layer
void fc_layer_destroy(FCLayer *layer)
{
    matrix_destroy(layer->weights);
    matrix_destroy(layer->biases);
    matrix_destroy(layer->activations);
    matrix_destroy(layer->deltas);
    matrix_destroy(layer->weights_gradient);
    matrix_destroy(layer->biases_gradient);
    free(layer);
}

#pragma endregion fully_connected_layer

#pragma region convolutional_layer

// convolutional layer
struct ConvLayer
{
    int input_height;
    int input_width;
    int input_depth;

    int output_height;
    int output_width;
    int n_filters;

    int kernel_size;
    int stride;
    int padding;
    float (*activation_func)(float);
    Matrix4 *weights;
    Matrix *biases;
    Matrix4 *activations;
    Matrix4 *deltas;
    Matrix4 *weights_gradient;
    Matrix *biases_gradient;
};
typedef struct ConvLayer ConvLayer;

// initialize a weight matrix
Matrix4 *conv_weight_init(int dim1, int dim2, int dim3, int dim4)
{
    Matrix4 *weight = matrix4_init(dim1, dim2, dim3, dim4, NULL);
    for (int i = 0; i < dim1; i++)
        for (int j = 0; j < dim2; j++)
            for (int k = 0; k < dim3; k++)
                for (int l = 0; l < dim4; l++)
                    weight->data[i][j][k][l] = (float)rand() / (float)(RAND_MAX / 2) - 1;
    return weight;
}

// initialize a bias matrix
Matrix *conv_bias_init(int dim1, int dim2)
{
    Matrix *bias = matrix_init(dim1, dim2, NULL);
    for (int i = 0; i < dim1; i++)
        for (int j = 0; j < dim2; j++)
            bias->data[i][j] = 0;
    return bias;
}

// create a new convolutional layer
ConvLayer *conv_layer_init(int input_height, int input_width, int input_depth, int n_filters, int kernel_size, int stride, int padding, int batch_size, float (*activation_func)(float), bool load_weights)
{
    ConvLayer *layer = malloc(sizeof(ConvLayer));
    layer->input_height = input_height;
    layer->input_width = input_width;
    layer->input_depth = input_depth;

    layer->output_height = (input_height - kernel_size + 2 * padding) / stride + 1;
    layer->output_width = (input_width - kernel_size + 2 * padding) / stride + 1;
    layer->n_filters = n_filters;

    layer->kernel_size = kernel_size;
    layer->stride = stride;
    layer->padding = padding;
    layer->activation_func = activation_func;

    // initialize weights and biases
    if (load_weights)
    {
        // parser for weights
    }
    else
    {
        layer->weights = conv_weight_init(n_filters, input_depth, kernel_size, kernel_size);
        layer->biases = conv_bias_init(n_filters, 1);
    }

    // initialize activations and deltas
    layer->activations = matrix4_init(batch_size, n_filters, layer->output_height, layer->output_width, NULL);
    layer->deltas = matrix4_init(batch_size, n_filters, layer->output_height, layer->output_width, NULL);

    // initialize gradients
    layer->weights_gradient = matrix4_init(n_filters, input_depth, kernel_size, kernel_size, NULL);
    layer->biases_gradient = matrix_init(n_filters, 1, NULL);

    return layer;
}

// forward pass for an input of shape: (batch_size, height, width, depth)
void conv_layer_forward(ConvLayer *layer, Matrix4 *input)
{
    // calculate activations
    matrix4_convolve(layer->weights, input, layer->activations, layer->stride, layer->padding);
    matrix4_matrix_add(layer->activations, layer->biases, layer->activations);
    matrix4_map_function(layer->activations, layer->activation_func);
}

// TODO: backprop

#pragma endregion convolutional_layer