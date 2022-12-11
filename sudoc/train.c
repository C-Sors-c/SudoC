#include "include/utils.h"
#include "include/matrix.h"
#include "include/layer.h"
#include "include/neuralnet.h"
#include "include/cv.h"
#include <string.h>

NN *build_nn(int batchsize)
{
    // define the layers
    FCLayer **fc_layers = malloc(sizeof(FCLayer) * 2);
    fc_layers[0] = fc_layer_init(28 * 28, 256, batchsize, relu, d_relu, "fc0");
    fc_layers[1] = fc_layer_init(256, 128, batchsize, relu, d_relu, "fc1");
    fc_layers[2] = fc_layer_init(128, 10, batchsize, relu, d_relu, "fc2");

    ActivationLayer *output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);
    int num_fc_layers = 3;

    NN *network = nn_init(fc_layers, num_fc_layers, output_layer);
    return network;
}

int main()
{
    init_rand();
    int batchsize = 1;

    NN *network = build_nn(batchsize);

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

    // load the weights: 
    bool loaded = nn_load(network, "weights");
    if (!loaded)
    {
        printf("Failed to load the weights \n");
        return 0;
    }

    // test the model using 1000 samples
    int num_samples = 1000;
    int correct = 0;

    for (int i = 0; i < num_samples; i++)
    {
        int digit = rand() % 10;
        int file = rand() % data_count[digit];

        CV_LOAD_MAT(filepaths[digit][file], input, batchsize, 1);

        int max_index = nn_predict(network, input);
        
        if (max_index == digit)
        {
            correct++;
        }
    }

    printf("Accuracy: %f \n", (float)correct / (float)num_samples);

    // free the memory
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