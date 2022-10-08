#pragma once

#include "matrix.h"
#include "weight_parser.h"
#include "layer.h"
#include <stdio.h>

struct NeuralNetwork {
    ConvLayer **conv_layers;
    FCLayer **fc_layers;
    ActivationLayer *output_layer;
};
typedef struct NeuralNetwork NeuralNetwork;