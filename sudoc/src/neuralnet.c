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

int *nn_predict(NN *neural_network, Matrix *input)
{
    Matrix *predictions = nn_forward(neural_network, input);
    int *pred = matrix_argmax(predictions);
    matrix_destroy(predictions);
    return pred;
}

void nn_backward(NN *neural_network, Matrix *input, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *loss_deltas = matrix_subtract(predictions, labels, NULL);
    Matrix *deltas = activation_layer_backward(neural_network->output_layer, loss_deltas);

    for (int i = neural_network->num_fc_layers - 1; i > 0; i--)
    {
        deltas = fc_layer_backward(neural_network->fc_layers[i], neural_network->fc_layers[i - 1]->activations, deltas, learning_rate);
    }
    deltas = fc_layer_backward(neural_network->fc_layers[0], input, deltas, learning_rate);

    matrix_destroy(loss_deltas);
}

double nn_train_batch(NN *neural_network, Matrix *input, Matrix *labels, float learning_rate)
{
    Matrix *predictions = nn_forward(neural_network, input);
    // matrix_print(predictions);
    double loss = cross_entropy_loss(predictions, labels);
    nn_backward(neural_network, input, predictions, labels, learning_rate);
    matrix_destroy(predictions);
    return loss;
}

void nn_destroy(NN *neural_network)
{
    for (int i = 0; i < neural_network->num_fc_layers; i++)
        fc_layer_destroy(neural_network->fc_layers[i]);
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
    for (int i = 0; i < neural_network->num_conv_layers; i++)
        input = conv_layer_forward(neural_network->conv_layers[i], input);

    Matrix *flattenned = matrix4_flatten(input, NULL);
    Matrix *y = flattenned; // keep track of the matrix so we can free it later

    for (int i = 0; i < neural_network->num_fc_layers; i++)
        y = fc_layer_forward(neural_network->fc_layers[i], y);

    y = activation_layer_forward(neural_network->output_layer, y);

    matrix_destroy(flattenned);
    return matrix_copy(y, NULL);
}

void cnn_backward(CNN *neural_network, Matrix4 *input, Matrix *predictions, Matrix *labels, float learning_rate)
{
    Matrix *loss_deltas = matrix_subtract(predictions, labels, NULL);
    Matrix *deltas = activation_layer_backward(neural_network->output_layer, loss_deltas);

    for (int i = neural_network->num_fc_layers - 1; i > 0; i--)
        deltas = fc_layer_backward(neural_network->fc_layers[i], neural_network->fc_layers[i - 1]->activations, deltas, learning_rate);

    // fc_input is the output of conv layers forward
    Matrix *fc_input = matrix4_flatten(neural_network->conv_layers[neural_network->num_conv_layers - 1]->activations, NULL);
    deltas = fc_layer_backward(neural_network->fc_layers[0], fc_input, deltas, learning_rate);

    ConvLayer *last_conv_layer = neural_network->conv_layers[neural_network->num_conv_layers - 1];

    Matrix4 *deltas4 = matrix4_unflatten(deltas, last_conv_layer->outgrad);
    for (int i = neural_network->num_conv_layers - 1; i > 0; i--)
        deltas4 = conv_layer_backward(neural_network->conv_layers[i], neural_network->conv_layers[i - 1]->activations, deltas4, learning_rate);

    deltas4 = conv_layer_backward(neural_network->conv_layers[0], input, deltas4, learning_rate);

    matrix_destroy(fc_input);
    matrix_destroy(loss_deltas);
}

double cnn_train_batch(CNN *neural_network, Matrix4 *input, Matrix *labels, float learning_rate)
{
    Matrix *predictions = cnn_forward(neural_network, input);
    // matrix_print(predictions);
    double loss = mean_squared_error(predictions, labels);
    // printf("loss: %f\n", loss);
    cnn_backward(neural_network, input, predictions, labels, learning_rate);
    matrix_destroy(predictions);
    return loss;
}

void cnn_destroy(CNN *neural_network)
{

    for (int i = 0; i < neural_network->num_conv_layers; i++)
        conv_layer_destroy(neural_network->conv_layers[i]);
    for (int i = 0; i < neural_network->num_fc_layers; i++)
        fc_layer_destroy(neural_network->fc_layers[i]);
    activation_layer_destroy(neural_network->output_layer);

    free(neural_network->conv_layers);
    free(neural_network->fc_layers);
    free(neural_network);
}

#pragma endregion cnn

#pragma region load_save

void fc_layer_save_weights(const char *filename, FCLayer *layer)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        err(1, "save_weight: fopen");
    }

    fprintf(fp, "%d %d\n", layer->weights->dim1, layer->weights->dim2);

    // write the weight matrix
    for (int i = 0; i < layer->weights->size - 1; i++)
    {
        fprintf(fp, "%f ", layer->weights->data[i]);
    }
    fprintf(fp, "%f\n", layer->weights->data[layer->weights->size - 1]);

    // write the bias matrix
    for (int i = 0; i < layer->biases->size - 1; i++)
    {
        fprintf(fp, "%f ", layer->biases->data[i]);
    }
    fprintf(fp, "%f\n", layer->biases->data[layer->biases->size - 1]);

    fclose(fp);
}

bool fc_layer_load_weights(const char *filename, FCLayer *layer)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return false;

    int dim1, dim2;
    int ret = fscanf(fp, "%d %d", &dim1, &dim2);
    if (ret != 2)
        return false;

    if (dim1 != layer->weights->dim1 || dim2 != layer->weights->dim2)
    {
        printf("dim1: %d, dim2: %d\n", dim1, dim2);
        // matrix_printshape(layer->weights);
        errx(1, "fc_load_weight: weights matrix dimensions do not match");
    }

    // read the weight matrix
    for (int i = 0; i < layer->weights->size; i++)
    {
        if (fscanf(fp, "%f", &layer->weights->data[i]) != 1)
            return false;
    }

    // read the bias matrix
    for (int i = 0; i < layer->biases->size; i++)
    {
        if (fscanf(fp, "%f", &layer->biases->data[i]) != 1)
            return false;
    }

    fclose(fp);
    return true;
}

void conv_layer_save_weigths(const char *filename, ConvLayer *layer)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        err(1, "save_weight: fopen");
    }

    fprintf(fp, "%d %d %d %d\n", layer->weights->dim1, layer->weights->dim2, layer->weights->dim3, layer->weights->dim4);

    // write the weight matrix
    for (int i = 0; i < layer->weights->size - 1; i++)
    {
        fprintf(fp, "%f ", layer->weights->data[i]);
    }
    fprintf(fp, "%f\n", layer->weights->data[layer->weights->size - 1]);

    // write the bias matrix
    for (int i = 0; i < layer->biases->size - 1; i++)
    {
        fprintf(fp, "%f ", layer->biases->data[i]);
    }
    fprintf(fp, "%f\n", layer->biases->data[layer->biases->size - 1]);

    fclose(fp);
}

bool conv_layer_load_weights(const char *filename, ConvLayer *layer)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return false;

    int dim1, dim2, dim3, dim4;
    int ret = fscanf(fp, "%d %d %d %d", &dim1, &dim2, &dim3, &dim4);
    if (ret != 4)
        return false;

    if (dim1 != layer->weights->dim1 || dim2 != layer->weights->dim2 || dim3 != layer->weights->dim3 || dim4 != layer->weights->dim4)
    {
        errx(1, "conv_load_weight: weights matrix dimensions do not match");
    }

    // read the weight matrix
    for (int i = 0; i < layer->weights->size; i++)
    {
        if (fscanf(fp, "%f", &layer->weights->data[i]) != 1)
            return false;
    }

    // read the bias matrix
    for (int i = 0; i < layer->biases->size; i++)
    {
        if (fscanf(fp, "%f", &layer->biases->data[i]) != 1)
            return false;
    }

    fclose(fp);
    return true;
}

void nn_save(NN *neural_network, const char *basename)
{
    // create the directory
    mkdir(basename, 0777);

    for (int i = 0; i < neural_network->num_fc_layers; i++)
    {
        char filename[256];
        // sprintf(filename, "%s/fc_%d.weights", basename, i); // sprintf is unsafe
        snprintf(filename, sizeof(filename), "%s/fc_%d.weights", basename, i);
        fc_layer_save_weights(filename, neural_network->fc_layers[i]);
    }
}

bool nn_load(NN *neural_network, const char *basename)
{
    for (int i = 0; i < neural_network->num_fc_layers; i++)
    {
        char filename[256];
        // sprintf(filename, "%s/fc_%d.weights", basename, i); // sprintf is unsafe
        snprintf(filename, sizeof(filename), "%s/fc_%d.weights", basename, i);
        if (!fc_layer_load_weights(filename, neural_network->fc_layers[i]))
            return false;
    }
    return true;
}

void cnn_save(CNN *cnn, const char *basename)
{
    // create the directory
    mkdir(basename, 0777);

    for (int i = 0; i < cnn->num_conv_layers; i++)
    {
        char filename[256];
        // sprintf(filename, "%s/conv_%d.weights", basename, i); // sprintf is unsafe
        snprintf(filename, sizeof(filename), "%s/conv_%d.weights", basename, i);
        conv_layer_save_weigths(filename, cnn->conv_layers[i]);
    }

    for (int i = 0; i < cnn->num_fc_layers; i++)
    {
        char filename[256];
        // sprintf(filename, "%s/fc_%d.weights", basename, i); // sprintf is unsafe
        snprintf(filename, sizeof(filename), "%s/fc_%d.weights", basename, i);
        fc_layer_save_weights(filename, cnn->fc_layers[i]);
    }
}

bool cnn_load(CNN *cnn, const char *basename)
{
    for (int i = 0; i < cnn->num_conv_layers; i++)
    {
        char filename[256];
        // sprintf(filename, "%s/conv_%d.weights", basename, i); // sprintf is unsafe
        snprintf(filename, sizeof(filename), "%s/conv_%d.weights", basename, i);
        if (!conv_layer_load_weights(filename, cnn->conv_layers[i]))
            return false;
    }

    for (int i = 0; i < cnn->num_fc_layers; i++)
    {
        char filename[256];
        // sprintf(filename, "%s/fc_%d.weights", basename, i); // sprintf is unsafe
        snprintf(filename, sizeof(filename), "%s/fc_%d.weights", basename, i);
        if (!fc_layer_load_weights(filename, cnn->fc_layers[i]))
            return false;
    }
    return true;
}

#pragma endregion load_save