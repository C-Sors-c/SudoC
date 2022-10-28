#pragma once

#include <sys/stat.h>
#include "matrix.h"
#include "layer.h"


struct CNN
{
    ConvLayer **conv_layers;
    int num_conv_layers;
    FCLayer **fc_layers;
    int num_fc_layers;
    ActivationLayer *output_layer;
};
typedef struct CNN CNN;

CNN *cnn_init(ConvLayer **conv_layers, int num_conv_layers,
              FCLayer **fc_layers, int num_fc_layers,
              ActivationLayer *output_layer);
Matrix *cnn_forward(CNN *network, Matrix4 *input);
// void cnn_backward(CNN *network, Matrix4 *input, Matrix *predictions, Matrix *labels, float learning_rate);
float cnn_train_batch(CNN *network, Matrix4 *input, Matrix *expected, float learning_rate);
void cnn_destroy(CNN *network);

struct NN
{
    FCLayer **fc_layers;
    int num_fc_layers;
    ActivationLayer *output_layer;
};
typedef struct NN NN;

NN *nn_init(FCLayer **fc_layer, int num_fc_layers, ActivationLayer *output_layer);
Matrix *nn_forward(NN *network, Matrix *input);
void nn_backward(NN *network, Matrix *input, Matrix *predictions, Matrix *labels, float learning_rate);
float nn_train_batch(NN *network, Matrix *input, Matrix *expected, float learning_rate);
void nn_destroy(NN *network);


void fc_layer_save_weights(const char *filename, FCLayer *layer);
bool fc_layer_load_weights(const char *filename, FCLayer *layer);
void nn_save(NN *network, const char *basename);
bool nn_load(NN *network, const char *basename);
