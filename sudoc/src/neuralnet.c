#include "../include/neuralnet.h"

NeuralNetwork *init_neural_network(int batchsize, bool load_weights)
{
    NeuralNetwork *neural_network = malloc(sizeof(NeuralNetwork));

    // initialize convolutional layers
    neural_network->conv_layers = malloc(sizeof(ConvLayer) * 2);
    neural_network->conv_layers[0] = conv_layer_init(28, 28, 1, 24, 5, 2, 2, batchsize, leaky_relu, d_leaky_relu, load_weights);
    neural_network->conv_layers[1] = conv_layer_init(14, 14, 24, 32, 5, 2, 2, batchsize, leaky_relu, d_leaky_relu, load_weights);

    // shape at this point: (batch_size, 32, 7, 7)

    // initialize fully connected layers
    neural_network->fc_layers = malloc(sizeof(FCLayer) * 2);
    neural_network->fc_layers[0] = fc_layer_init(3136, 128, batchsize, leaky_relu, d_leaky_relu, load_weights);
    neural_network->fc_layers[1] = fc_layer_init(128, 10, batchsize, leaky_relu, d_leaky_relu, load_weights);

    // shape at this point: (batch_size, 10)

    // initialize output layer
    neural_network->output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);

    return neural_network;
}

// forward pass
Matrix *neural_network_forward(NeuralNetwork *neural_network, Matrix4 *input)
{
    Matrix4 *x = conv_layer_forward(neural_network->conv_layers[0], input);
    x = conv_layer_forward(neural_network->conv_layers[1], x);

    Matrix *y = matrix4_flatten(x);
    y = fc_layer_forward(neural_network->fc_layers[0], y);
    y = fc_layer_forward(neural_network->fc_layers[1], y);
    y = activation_layer_forward(neural_network->output_layer, y);

    return y;
}

// backward pass
void neural_network_backward(NeuralNetwork *neural_network, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *deltas = d_cross_entropy_loss(predictions, labels);
    deltas = activation_layer_backward(neural_network->output_layer, deltas);
    deltas = fc_layer_backward(neural_network->fc_layers[1], neural_network->fc_layers[1]->activations, deltas, learning_rate);
    deltas = fc_layer_backward(neural_network->fc_layers[0], neural_network->fc_layers[0]->activations, deltas, learning_rate);
    Matrix4 *deltas4 = matrix4_unflatten(deltas, NULL);
    deltas4 = conv_layer_backward(neural_network->conv_layers[1], neural_network->conv_layers[1]->activations, deltas4, learning_rate);
    deltas4 = conv_layer_backward(neural_network->conv_layers[0], neural_network->conv_layers[0]->activations, deltas4, learning_rate);

    free_matrix(deltas);
    free_matrix4(deltas4);
}

float neural_network_train_batch(NeuralNetwork *neural_network, Matrix4 *input, Matrix *labels, float learning_rate)
{
    Matrix *predictions = neural_network_forward(neural_network, input);
    float loss = cross_entropy_loss(predictions, labels);
    neural_network_backward(neural_network, predictions, labels, learning_rate);
    free_matrix(predictions);
    return loss;
}

void neural_network_destroy(NeuralNetwork *neural_network)
{
    conv_layer_destroy(neural_network->conv_layers[0]);
    conv_layer_destroy(neural_network->conv_layers[1]);
    fc_layer_destroy(neural_network->fc_layers[0]);
    fc_layer_destroy(neural_network->fc_layers[1]);
    activation_layer_destroy(neural_network->output_layer);

    free(neural_network->conv_layers);
    free(neural_network->fc_layers);
    free(neural_network);
}