#include "../include/neuralnet.h"

#pragma region cnn

CNN *init_neural_network(int batchsize, bool load_weights)
{
    CNN *neural_network = malloc(sizeof(CNN));

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
void cnn_backward(CNN *neural_network, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *deltas = matrix_subtract(predictions, labels, NULL);
    deltas = activation_layer_backward(neural_network->output_layer, deltas);
    // TODO: implement this + find a way to get the input to the fc layer (maybe store it in the layer struct?) some kind of pointer to the input

    // deltas = fc_layer_backward(neural_network->fc_layers[1], neural_network->fc_layers[0]->activations, deltas, learning_rate);
    // deltas = fc_layer_backward(neural_network->fc_layers[0], neural_network->fc_layers[0]->activations, deltas, learning_rate);
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
    cnn_backward(neural_network, predictions, labels, learning_rate);
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

#pragma region nnxor

NNXor *nnxor_init(int batchsize, bool load_weights)
{
    NNXor *neural_network = malloc(sizeof(NNXor));

    neural_network->fc_layers = malloc(2 * sizeof(FCLayer));
    neural_network->fc_layers[0] = fc_layer_init(2, 32, batchsize, leaky_relu, d_leaky_relu, load_weights);
    neural_network->fc_layers[1] = fc_layer_init(32, 32, batchsize, leaky_relu, d_leaky_relu, load_weights);
    neural_network->fc_layers[2] = fc_layer_init(32, 2, batchsize, leaky_relu, d_leaky_relu, load_weights);

    neural_network->output_layer = activation_layer_init(2, batchsize, softmax, d_softmax);

    return neural_network;
}

Matrix *nnxor_forward(NNXor *neural_network, Matrix *input)
{
    Matrix *y = fc_layer_forward(neural_network->fc_layers[0], input);
    y = fc_layer_forward(neural_network->fc_layers[1], y);
    y = fc_layer_forward(neural_network->fc_layers[2], y);
    y = activation_layer_forward(neural_network->output_layer, y);
    return matrix_copy(y, NULL);
}

void nnxor_backward(NNXor *neural_network, Matrix *input, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *loss_deltas = matrix_subtract(predictions, labels, NULL);
    Matrix *deltas = activation_layer_backward(neural_network->output_layer, loss_deltas);
    deltas = fc_layer_backward(neural_network->fc_layers[2], neural_network->fc_layers[1]->activations, deltas, learning_rate);
    deltas = fc_layer_backward(neural_network->fc_layers[1], neural_network->fc_layers[0]->activations, deltas, learning_rate);
    deltas = fc_layer_backward(neural_network->fc_layers[0], input, deltas, learning_rate);

    matrix_destroy(loss_deltas);
}

float nnxor_train_batch(NNXor *neural_network, Matrix *input, Matrix *labels, float learning_rate)
{
    Matrix *predictions = nnxor_forward(neural_network, input);
    float loss = cross_entropy_loss(predictions, labels);
    nnxor_backward(neural_network, input, predictions, labels, learning_rate);
    matrix_destroy(predictions);
    return loss;
}

void nnxor_destroy(NNXor *neural_network)
{
    fc_layer_destroy(neural_network->fc_layers[0]);
    fc_layer_destroy(neural_network->fc_layers[1]);
    fc_layer_destroy(neural_network->fc_layers[2]);
    activation_layer_destroy(neural_network->output_layer);

    free(neural_network->fc_layers);
    free(neural_network);
}

#pragma endregion nnxor