#pragma once

#include "matrix.h"

// Fully connected layer
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

Matrix *fc_weight_init(int dim1, int dim2);
Matrix *fc_bias_init(int dim1, int dim2);
FCLayer *fc_layer_init(
    int input_size, int output_size, int batch_size,
    float (*activation_func)(float), float (*d_activation_func)(float), bool load_weights);
void fc_layer_forward(FCLayer *layer, Matrix *input);
void fc_layer_backward(FCLayer *layer, Matrix *previous_activations, Matrix *previous_deltas);
void fc_layer_destroy(FCLayer *layer);

// Convolution layer
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
    float (*d_activation_func)(float);

    Matrix4 *weights;
    Matrix *biases;
    Matrix4 *activations;
    Matrix4 *deltas;
    Matrix4 *weights_gradient;
    Matrix *biases_gradient;
};
typedef struct ConvLayer ConvLayer;

Matrix4 *conv_weight_init(int dim1, int dim2, int dim3, int dim4);
Matrix *conv_bias_init(int dim1, int dim2);
ConvLayer *conv_layer_init(
    int input_height, int input_width, int input_depth,
    int n_filters, int kernel_size, int stride, int padding, int batch_size,
    float (*activation_func)(float), float (*d_activation_func)(float), bool load_weights);

// activations
float sigmoid(float x);
float d_sigmoid(float x);
float relu(float x);
float d_relu(float x);
float tanh(float x);
float d_tanh(float x);
float leaky_relu(float x);
float d_leaky_relu(float x);
Matrix *softmax(Matrix *m1);
Matrix *d_softmax(Matrix *m1);