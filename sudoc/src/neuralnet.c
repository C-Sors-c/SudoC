#include "../include/neuralnet.h"

#pragma region nn

NN *nn_init(FCLayer **fc_layer, int num_fc_layers, ActivationLayer *output_layer)
{
    NN *neural_network = malloc(sizeof(NN));

    neural_network->fc_layers = fc_layer;
    neural_network->num_fc_layers = num_fc_layers;
    neural_network->output_layer = output_layer;

    return neural_network;
}

Matrix *nn_forward(NN *neural_network, Matrix *input)
{   
    for (int i = 0; i < neural_network->num_fc_layers; i++)
        input = fc_layer_forward(neural_network->fc_layers[i], input);

    Matrix *y = activation_layer_forward(neural_network->output_layer, input);
    return matrix_copy(y, NULL);
}

void nn_backward(NN *neural_network, Matrix *input, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *loss_deltas = matrix_subtract(predictions, labels, NULL);
    Matrix *deltas = activation_layer_backward(neural_network->output_layer, loss_deltas);

    for (int i = neural_network->num_fc_layers - 1; i > 0; i--)
        deltas = fc_layer_backward(neural_network->fc_layers[i], neural_network->fc_layers[i - 1]->activations, deltas, learning_rate);
    deltas = fc_layer_backward(neural_network->fc_layers[0], input, deltas, learning_rate);

    matrix_destroy(loss_deltas);
}

float nn_train_batch(NN *neural_network, Matrix *input, Matrix *labels, float learning_rate)
{
    Matrix *predictions = nn_forward(neural_network, input);
    float loss = cross_entropy_loss(predictions, labels);
    nn_backward(neural_network, input, predictions, labels, learning_rate);
    matrix_destroy(predictions);
    return loss;
}

void nn_destroy(NN *neural_network)
{
    fc_layer_destroy(neural_network->fc_layers[0]);
    fc_layer_destroy(neural_network->fc_layers[1]);
    fc_layer_destroy(neural_network->fc_layers[2]);
    activation_layer_destroy(neural_network->output_layer);

    free(neural_network->fc_layers);
    free(neural_network);
}

#pragma endregion nn


#pragma region cnn

CNN *cnn_init(ConvLayer **conv_layers, int num_conv_layers,
              FCLayer **fc_layers, int num_fc_layers,
              ActivationLayer *output_layer)
{
    CNN *neural_network = malloc(sizeof(CNN));

    // initialize convolutional layers
    neural_network->conv_layers = conv_layers;
    neural_network->num_conv_layers = num_conv_layers;

    // initialize fully connected layers
    neural_network->fc_layers = fc_layers;
    neural_network->num_fc_layers = num_fc_layers;

    // initialize output layer
    neural_network->output_layer = output_layer;

    return neural_network;
}

// forward pass
Matrix *cnn_forward(CNN *neural_network, Matrix4 *input)
{
    Matrix4 *x = conv_layer_forward(neural_network->conv_layers[0], input);
    x = conv_layer_forward(neural_network->conv_layers[1], x);

    // I think there will be mem leak here, need to move to a proper layer
    Matrix *y = matrix4_flatten(x, NULL);
    y = fc_layer_forward(neural_network->fc_layers[0], y);
    y = fc_layer_forward(neural_network->fc_layers[1], y);
    y = activation_layer_forward(neural_network->output_layer, y);

    return y;
}

// backward pass
void cnn_backward(CNN *neural_network, Matrix4 *input, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *loss_deltas = matrix_subtract(predictions, labels, NULL);
    Matrix *deltas = activation_layer_backward(neural_network->output_layer, loss_deltas);

    for (int i = neural_network->num_fc_layers - 1; i > 0; i--)
        deltas = fc_layer_backward(neural_network->fc_layers[i], neural_network->fc_layers[i - 1]->activations, deltas, learning_rate);
    // deltas = fc_layer_backward(neural_network->fc_layers[0], fc_input, deltas, learning_rate);


    // Matrix4 *deltas4 = matrix4_unflatten(deltas, NULL);
    // deltas4 = conv_layer_backward(neural_network->conv_layers[1], neural_network->conv_layers[0]->activations, deltas4, learning_rate);
    // deltas4 = conv_layer_backward(neural_network->conv_layers[0], neural_network->conv_layers[0]->activations, deltas4, learning_rate);

    matrix_destroy(deltas);
    // matrix4_destroy(deltas4);
}

float cnn_train_batch(CNN *neural_network, Matrix4 *input, Matrix *labels, float learning_rate)
{
    Matrix *predictions = cnn_forward(neural_network, input);
    float loss = cross_entropy_loss(predictions, labels);
    cnn_backward(neural_network, input, predictions, labels, learning_rate);
    matrix_destroy(predictions);
    return loss;
}

void cnn_destroy(CNN *neural_network)
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

#pragma endregion cnn
