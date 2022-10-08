#pragma once

#include "matrix.h"
#include "weight_parser.h"
#include "layer.h"

struct NeuralNetwork {
    ConvLayer **conv_layers;
    FCLayer **fc_layers;
    ActivationLayer *output_layer;
};
typedef struct NeuralNetwork NeuralNetwork;

NeuralNetwork *neural_network_init(int batch_size, bool load_weights);
Matrix * neural_network_forward(NeuralNetwork *network, Matrix4 *input);
void neural_network_backward(NeuralNetwork *network, Matrix *predictions, Matrix *labels, float learning_rate);
float neural_network_train_batch(NeuralNetwork *network, Matrix4 *input, Matrix *expected, float learning_rate);
void neural_network_destroy(NeuralNetwork *network);
