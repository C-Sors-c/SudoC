#pragma once

#include "matrix.h"
#include "weight_parser.h"
#include "layer.h"

struct CNN {
    ConvLayer **conv_layers;
    FCLayer **fc_layers;
    ActivationLayer *output_layer;
};
typedef struct CNN CNN;

CNN *cnn_init(int batch_size, bool load_weights);
Matrix *cnn_forward(CNN *network, Matrix4 *input);
void cnn_backward(CNN *network, Matrix *predictions, Matrix *labels, float learning_rate);
float cnn_train_batch(CNN *network, Matrix4 *input, Matrix *expected, float learning_rate);
void cnn_destroy(CNN *network);


struct NNXor {
    FCLayer **fc_layers;
    ActivationLayer *output_layer;
};
typedef struct NNXor NNXor;

NNXor *nnxor_init(int batchsize, bool load_weights);
Matrix *nnxor_forward(NNXor *network, Matrix *input);
void nnxor_backward(NNXor *network, Matrix *input, Matrix *predictions, Matrix *labels, float learning_rate);
float nnxor_train_batch(NNXor *network, Matrix *input, Matrix *expected, float learning_rate);
void nnxor_destroy(NNXor *network);