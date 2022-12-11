#include "include/utils.h"
#include "include/matrix.h"
#include "include/layer.h"
#include "include/neuralnet.h"
#include "include/cv.h"
#include <string.h>

CNN *build_cnn(int batchsize)
{
    // define the layers
    ConvLayer **conv_layers = malloc(sizeof(ConvLayer) * 1);
    conv_layers[0] = conv_layer_init(28, 28, 1, 1, 1, 1, 0, batchsize, sigmoid, d_sigmoid, "conv0");

    FCLayer **fc_layers = malloc(sizeof(FCLayer) * 2);
    fc_layers[0] = fc_layer_init(28 * 28 * 1, 128, batchsize, sigmoid, d_sigmoid, "fc0");
    fc_layers[1] = fc_layer_init(128, 10, batchsize, sigmoid, d_sigmoid, "fc1");

    ActivationLayer *output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);
    int num_conv_layers = 1;
    int num_fc_layers = 2;

    CNN *network = cnn_init(conv_layers, num_conv_layers, fc_layers, num_fc_layers, output_layer);
    return network;
}

NN *build_nn(int batchsize)
{
    // define the layers
    FCLayer **fc_layers = malloc(sizeof(FCLayer) * 2);
    fc_layers[0] = fc_layer_init(28 * 28, 128, batchsize, sigmoid, d_sigmoid, "fc0");
    fc_layers[1] = fc_layer_init(128, 128, batchsize, sigmoid, d_sigmoid, "fc1");
    fc_layers[1] = fc_layer_init(128, 10, batchsize, sigmoid, d_sigmoid, "fc2");

    ActivationLayer *output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);
    int num_fc_layers = 2;

    NN *network = nn_init(fc_layers, num_fc_layers, output_layer);
    return network;
}

int main()
{
    init_rand();
    int batchsize = 4;

    // CNN *network = build_cnn(batchsize);
    NN *network = build_nn(batchsize);

    // Matrix4 *input = matrix4_init(batchsize, 1, 28, 28, NULL);
    Matrix *input = matrix_init(batchsize, 28 * 28, NULL);
    Matrix *output = matrix_init(batchsize, 10, NULL);

    int *data_count = malloc(sizeof(int) * 10);
    char ***filepaths = malloc(sizeof(char **) * 10);

    for (int i = 0; i < 10; i++)
    {
        data_count[i] = 0;

        char *path = malloc(sizeof(char) * 100);
        sprintf(path, "train_data/%d", i);

        filepaths[i] = CV_LIST_DIR(path, &data_count[i]);

        free(path);
    }

    int epochs = 10;
    for (int e = 0; e < epochs; e++)
    {
        for (int k = 0; k < 2500; k++)
        {
            for (int i = 0; i < batchsize; i++)
            {
                int digit = rand() % 10;
                int file = rand() % data_count[digit];

                CV_LOAD_MAT(filepaths[digit][file], input, i, batchsize, 1);

                for (int j = 0; j < 10; j++)
                {
                    m_set(output, i, j, 0.0f);
                }
                m_set(output, i, digit, 1.0f);
            }

            // matrix_print(input);

            double loss = nn_train_batch(network, input, output, 0.001f);
            // printf("output: \n");
            // matrix_print(output);
            // printf("Epoch %d, loss: %f \n", e, loss);

            if (k % 1000 == 0)
            {
                nn_save(network, "cnn-weight");
            }
        }
    }

    // free the memory
    // cnn_destroy(network);
    nn_destroy(network);
    matrix_destroy(input);
    matrix_destroy(output);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < data_count[i]; j++)
        {
            free(filepaths[i][j]);
        }
        free(filepaths[i]);
    }
    free(filepaths);
    free(data_count);
}