
#include "../include/layer.h"

#pragma region fully_connected_layer

// initialize a weight matrix
Matrix *fc_weight_init(int dim1, int dim2)
{
    Matrix *weight = matrix_init(dim1, dim2, NULL);
    for (int i = 0; i < weight->size; i++)
        weight->data[i] = (float)rand() / (float)(RAND_MAX / 2) - 1;
    return weight;
}

// initialize a bias matrix
Matrix *fc_bias_init(int dim1, int dim2)
{
    Matrix *bias = matrix_init(dim1, dim2, NULL);
    return bias;
}

// create a new fully connected layer
FCLayer *fc_layer_init(
    int input_size, int output_size, int batch_size,
    float (*activation_func)(float), float (*d_activation_func)(float),
    char *name)
{
    FCLayer *layer = malloc(sizeof(FCLayer));
    layer->name = name;

    layer->input_size = input_size;
    layer->output_size = output_size;
    layer->activation_func = activation_func;
    layer->d_activation_func = d_activation_func;

    // initialize weights and biases randomly
    layer->weights = fc_weight_init(output_size, input_size);
    layer->biases = fc_bias_init(1, output_size);

    // initialize activations and deltas
    layer->activations = matrix_init(batch_size, output_size, NULL);
    layer->deltas = matrix_init(batch_size, input_size, NULL);

    // initialize matrices for backprop
    layer->weights_gradient = matrix_init(output_size, input_size, NULL);
    layer->biases_gradient = matrix_init(1, output_size, NULL);

    return layer;
}

// forward pass for an input of shape: (batch_size, input_size)
Matrix *fc_layer_forward(FCLayer *layer, Matrix *input)
{
    // calculate activations
    Matrix *WT = matrix_transpose(layer->weights);
    matrix_multiply(input, WT, layer->activations);
    matrix_add_bias(layer->activations, layer->biases, layer->activations);
    matrix_map_function(layer->activations, layer->activation_func);

    matrix_destroy(WT);

    return layer->activations;
}

// backward pass for an input of shape: (batch_size, input_size)
//
// previous_activations: activations of the previous layer (input)
// previous_deltas: deltas of the next layer (output)
// learning_rate: learning rate

Matrix *fc_layer_backward(FCLayer *layer, Matrix *prev_activations, Matrix *prev_deltas, float learning_rate)
{
    Matrix *dZ = matrix_copy(layer->activations, NULL);
    matrix_map_function(dZ, layer->d_activation_func);
    matrix_elementwise_multiply(dZ, prev_deltas, dZ);

    Matrix *prev_activationsT = matrix_transpose(prev_activations);
    Matrix *dW = matrix_multiply(prev_activationsT, dZ, NULL);
    Matrix *db = matrix_sum_rows(dZ, NULL);
    matrix_multiply(dZ, layer->weights, layer->deltas);

    // update weights and biases
    matrix_multiply_scalar(dW, -learning_rate);
    matrix_multiply_scalar(db, -learning_rate);
    Matrix *dWT = matrix_transpose(dW);
    matrix_add(layer->weights, dWT, layer->weights);
    matrix_add(layer->biases, db, layer->biases);

    // free memory
    matrix_destroy(dZ);
    matrix_destroy(dW);
    matrix_destroy(db);
    matrix_destroy(dWT);
    matrix_destroy(prev_activationsT);

    return layer->deltas;
}

void fc_layer_print(FCLayer *layer)
{
    printf("input_size: %d, output_size: %d\n", layer->input_size, layer->output_size);
    printf("weights: dim1: %d, dim2: %d\n", layer->weights->dim1, layer->weights->dim2);
    printf("biases: dim1: %d, dim2: %d\n", layer->biases->dim1, layer->biases->dim2);
    printf("activations: dim1: %d, dim2: %d\n", layer->activations->dim1, layer->activations->dim2);
    printf("deltas: dim1: %d, dim2: %d\n", layer->deltas->dim1, layer->deltas->dim2);
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

// initialize a weight matrix
Matrix4 *conv_weight_init(int dim1, int dim2, int dim3, int dim4)
{
    Matrix4 *weight = matrix4_init(dim1, dim2, dim3, dim4, NULL);
    for (int i = 0; i < weight->size; i++)
        weight->data[i] = (float)rand() / (float)(RAND_MAX / 2) - 1;
    return weight;
}

// initialize a bias matrix
Matrix *conv_bias_init(int dim1, int dim2)
{
    Matrix *bias = matrix_init(dim1, dim2, NULL);
    return bias;
}

// create a new convolutional layer
ConvLayer *conv_layer_init(
    int input_height, int input_width, int input_depth,
    int n_filters, int kernel_size, int stride, int padding,
    int batch_size, float (*activation_func)(float), float (*d_activation_func)(float),
    bool load_weights, char *name)
{
    ConvLayer *layer = malloc(sizeof(ConvLayer));

    layer->name = name;

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
    layer->d_activation_func = d_activation_func;

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

// forward pass for an input of shape: (batch_size, depth, height, width)
Matrix4 *conv_layer_forward(ConvLayer *layer, Matrix4 *input)
{
    // calculate activations
    matrix4_convolve(layer->weights, input, layer->activations, layer->stride, layer->padding);
    matrix4_add_bias(layer->activations, layer->biases, layer->activations);
    matrix4_map_function(layer->activations, layer->activation_func);

    return layer->activations;
}

// backward pass
// Matrix4 *conv_layer_backward(ConvLayer *layer, Matrix4 *previous_activations, Matrix4 *previous_deltas, float learning_rate)
// {
//     // TODO: implement and test
//     return NULL;
// }

// print layer info
void conv_layer_print(ConvLayer *layer)
{
    printf("input_shap: (%d,%d,%d), output_size: (%d,%d,%d)\n", layer->input_height, layer->input_width, layer->input_depth, layer->output_height, layer->output_width, layer->n_filters);
    printf("weights: dim1: %d, dim2: %d, dim3: %d, dim4: %d\n", layer->weights->dim1, layer->weights->dim2, layer->weights->dim3, layer->weights->dim4);
    printf("biases: dim1: %d, dim2: %d\n", layer->biases->dim1, layer->biases->dim2);
    printf("activations: dim1: %d, dim2: %d, dim3: %d, dim4: %d\n", layer->activations->dim1, layer->activations->dim2, layer->activations->dim3, layer->activations->dim4);
    printf("deltas: dim1: %d, dim2: %d, dim3: %d, dim4: %d\n", layer->deltas->dim1, layer->deltas->dim2, layer->deltas->dim3, layer->deltas->dim4);
}

// destroy a convolutional layer
void conv_layer_destroy(ConvLayer *layer)
{
    matrix4_destroy(layer->weights);
    matrix_destroy(layer->biases);
    matrix4_destroy(layer->activations);
    matrix4_destroy(layer->deltas);
    matrix4_destroy(layer->weights_gradient);
    matrix_destroy(layer->biases_gradient);
    free(layer);
}

#pragma endregion convolutional_layer

#pragma region activations

float sigmoid(float x)
{
    return 1 / (1 + exp(-x));
}

float d_sigmoid(float x)
{
    return sigmoid(x) * (1 - sigmoid(x));
}

float relu(float x)
{
    return x > 0 ? x : 0;
}

float d_relu(float x)
{
    return x > 0 ? 1 : 0;
}

float leaky_relu(float x)
{
    return x > 0 ? x : 0.1 * x;
}

float d_leaky_relu(float x)
{
    return x > 0 ? 1 : 0.1;
}

Matrix *softmax(Matrix *m1)
{
    Matrix *dst = matrix_init(m1->dim1, m1->dim2, NULL);
    Matrix *m1norm = matrix_copy(m1, NULL);

    // max normalized m1
    for (int i = 0; i < m1->dim1; i++)
    {
        float max = m1->data[i * m1->dim2];
        for (int j = 0; j < m1->dim2; j++)
        {
            if (m1->data[i * m1->dim2 + j] > max)
                max = m1->data[i * m1->dim2 + j];
        }
        for (int j = 0; j < m1->dim2; j++)
        {
            m1norm->data[i * m1->dim2 + j] -= max;
        }
    }

    for (int i = 0; i < m1norm->dim1; i++)
    {
        float sum = 0;
        for (int j = 0; j < m1norm->dim2; j++)
            sum += exp(m1norm->data[i * m1norm->dim2 + j]);
        for (int j = 0; j < m1norm->dim2; j++)
            dst->data[i * m1norm->dim2 + j] = exp(m1norm->data[i * m1norm->dim2 + j]) / sum;
    }

    matrix_destroy(m1norm);
    return dst;
}

Matrix *d_softmax(Matrix *m1)
{
    Matrix *dst = matrix_init(m1->dim1, m1->dim2, NULL);
    for (int i = 0; i < m1->dim1; i++)
        for (int j = 0; j < m1->dim2; j++)
            dst->data[i * m1->dim2 + j] = m1->data[i * m1->dim2 + j] * (1 - m1->data[i * m1->dim2 + j]);
    return dst;
}

ActivationLayer *activation_layer_init(
    int input_size, int batch_size,
    Matrix *(*activation_func)(Matrix *), Matrix *(*d_activation_func)(Matrix *))
{
    ActivationLayer *layer = malloc(sizeof(ActivationLayer));
    layer->input_size = input_size;
    layer->batch_size = batch_size;
    layer->activation_func = activation_func;
    layer->d_activation_func = d_activation_func;
    layer->activations = matrix_init(batch_size, input_size, NULL);
    layer->deltas = matrix_init(batch_size, input_size, NULL);
    return layer;
}

Matrix *activation_layer_forward(ActivationLayer *layer, Matrix *input)
{
    Matrix *activations = layer->activation_func(input);
    matrix_copy(activations, layer->activations);
    matrix_destroy(activations);
    return layer->activations;
}

Matrix *activation_layer_backward(ActivationLayer *layer, Matrix *previous_deltas)
{
    Matrix *deltas = layer->d_activation_func(previous_deltas);
    matrix_copy(deltas, layer->deltas);
    matrix_destroy(deltas);
    return layer->deltas;
}

void activation_layer_destroy(ActivationLayer *layer)
{
    matrix_destroy(layer->activations);
    matrix_destroy(layer->deltas);
    free(layer);
}

#pragma endregion activations

#pragma region flatten

FlattenLayer *flatten_layer_init(int input_height, int input_width, int input_depth, int batch_size)
{
    FlattenLayer *layer = malloc(sizeof(FlattenLayer));
    layer->input_height = input_height;
    layer->input_width = input_width;
    layer->output_size = input_height * input_width * input_depth;
    layer->input_depth = input_depth;
    layer->batch_size = batch_size;
    layer->activations = matrix_init(batch_size, layer->output_size, NULL);
    layer->deltas = matrix4_init(batch_size, input_depth, input_height, input_width, NULL);
    return layer;
}

Matrix *flatten_layer_forward(FlattenLayer *layer, Matrix4 *input)
{
    return matrix4_flatten(input, layer->activations);
}

Matrix4 *flatten_layer_backward(FlattenLayer *layer, Matrix *previous_deltas)
{
    return matrix4_unflatten(previous_deltas, layer->deltas);
}

void flatten_layer_destroy(FlattenLayer *layer)
{
    matrix_destroy(layer->activations);
    matrix4_destroy(layer->deltas);
    free(layer);
}

#pragma endregion flatten

#pragma region loss

float cross_entropy_loss(Matrix *predictions, Matrix *labels)
{
    float loss = 0;
    for (int i = 0; i < predictions->size; i++)
        loss += labels->data[i] * log(predictions->data[i]);
    return -loss / predictions->dim1;
}

#pragma endregion loss