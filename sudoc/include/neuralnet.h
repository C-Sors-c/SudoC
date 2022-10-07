#pragma once

#include "matrix.h"
#include "weight_parser.h"
#include "layer.h"
#include <stdio.h>

struct NeuralNetwork {
    ConvLayer *conv_Layers;
    FCLayer *fc_layers;
};
typedef NeuralNetwork NeuralNetwork;