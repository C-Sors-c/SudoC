#include "../include/neuralnet.h"

NeuralNetwork *init_neural_network(bool load_weights)
{
    NeuralNetwork *neural_network = malloc(sizeof(NeuralNetwork));

    // initialize convolutional layers
    neural_network->conv_layers = malloc(sizeof(ConvLayer) * 2);
    neural_network->conv_layers[0] = conv_layer_init(28, 28, 1, 32, 5, 1, 2, 1, relu, d_relu, load_weights);
    neural_network->conv_layers[1] = conv_layer_init(28, 28, 32, 64, 5, 1, 2, 1, relu, d_relu, load_weights);

    // initialize fully connected layers
    neural_network->fc_layers = malloc(sizeof(FCLayer) * 2);
    neural_network->fc_layers[0] = fc_layer_init(3136, 1024, 1, relu, d_relu, load_weights);
    neural_network->fc_layers[1] = fc_layer_init(1024, 10, 1, relu, d_relu, load_weights);

}
