
#include "../include/test_nn.h"

int test_nnxor()
{
    init_rand();

    int batchsize = 4;

    // define the layers
    FCLayer **fc_layers = malloc(sizeof(FCLayer));
    fc_layers[0] = fc_layer_init(2, 32, batchsize, leaky_relu, d_leaky_relu, "fc0");
    fc_layers[1] = fc_layer_init(32, 32, batchsize, leaky_relu, d_leaky_relu, "fc1");
    fc_layers[2] = fc_layer_init(32, 2, batchsize, leaky_relu, d_leaky_relu, "fc2");
    ActivationLayer *output_layer = activation_layer_init(2, batchsize, softmax, d_softmax);
    int num_fc_layers = 3;

    NN *network = nn_init(fc_layers, num_fc_layers, output_layer);
    Matrix *input = matrix_init(batchsize, 2, NULL);
    Matrix *expected = matrix_init(batchsize, 2, NULL);

    // shape of input: (4, 2) (batchsize, size)
    // shape of expected: (4, 2) (batchsize, size)

    // 0 xor 0 = 0
    m_set(input, 0, 0, 0);
    m_set(input, 0, 1, 0);
    m_set(expected, 0, 0, 1);

    // 0 xor 1 = 1
    m_set(input, 1, 0, 0);
    m_set(input, 1, 1, 1);
    m_set(expected, 1, 1, 1);

    // 1 xor 0 = 1
    m_set(input, 2, 0, 1);
    m_set(input, 2, 1, 0);
    m_set(expected, 2, 1, 1);

    // 1 xor 1 = 0
    m_set(input, 3, 0, 1);
    m_set(input, 3, 1, 1);
    m_set(expected, 3, 0, 1);

    // test the network with different inputs
    // (to make sure there is no issue with batch order)

    Matrix *test_input = matrix_init(batchsize, 2, NULL);
    Matrix *test_expected = matrix_init(batchsize, 2, NULL);

    // 1 xor 0 = 1
    m_set(test_input, 0, 0, 1);
    m_set(test_input, 0, 1, 0);
    m_set(test_expected, 0, 1, 1);

    // 0 xor 0 = 0
    m_set(test_input, 1, 0, 0);
    m_set(test_input, 1, 1, 0);
    m_set(test_expected, 1, 0, 1);

    // 1 xor 1 = 0
    m_set(test_input, 2, 0, 1);
    m_set(test_input, 2, 1, 1);
    m_set(test_expected, 2, 0, 1);

    // 0 xor 1 = 1
    m_set(test_input, 3, 0, 0);
    m_set(test_input, 3, 1, 1);
    m_set(test_expected, 3, 1, 1);

    // train the network
    int correct = 0;
    float accuracy = 0;
    float min_accuracy = 0.95;
    int epoch = 10000;
    float learning_rate = 0.001;

    for (int i = 0; i < epoch && correct != 4; i++)
    {
        nn_train_batch(network, input, expected, learning_rate);
        Matrix *predictions = nn_forward(network, test_input);

        correct = 0;
        accuracy = 0;

        float p1 = m_get(predictions, 0, 1);
        float p2 = m_get(predictions, 1, 0);
        float p3 = m_get(predictions, 2, 0);
        float p4 = m_get(predictions, 3, 1);

        float tests[4] = {p1, p2, p3, p4};

        for (int j = 0; j < 4; j++)
            if (tests[j] > min_accuracy)
                correct++;

        accuracy = (p1 + p2 + p3 + p4) / 4.0;
        matrix_destroy(predictions);

        // printf("Accuracy: %f, Correct: %d, Sum: %f, Iteration: %d\n", accuracy, correct, sum, i);
    }

    // save weights
    nn_save(network, "tests/out/nnxor");

    nn_destroy(network);
    matrix_destroy(input);
    matrix_destroy(expected);
    matrix_destroy(test_input);
    matrix_destroy(test_expected);

    // printf("Accuracy: %f, Correct: %d\n", accuracy, correct);
    return assert(accuracy > min_accuracy, true, "test_nnxor");
}

int test_nnxor_load()
{

    int batchsize = 2;

    // define the layers
    FCLayer **fc_layers = malloc(sizeof(FCLayer));
    fc_layers[0] = fc_layer_init(2, 32, batchsize, leaky_relu, d_leaky_relu, "fc0");
    fc_layers[1] = fc_layer_init(32, 32, batchsize, leaky_relu, d_leaky_relu, "fc1");
    fc_layers[2] = fc_layer_init(32, 2, batchsize, leaky_relu, d_leaky_relu, "fc2");
    ActivationLayer *output_layer = activation_layer_init(2, batchsize, softmax, d_softmax);
    int num_fc_layers = 3;

    NN *network = nn_init(fc_layers, num_fc_layers, output_layer);

    bool ret = nn_load(network, "tests/out/nnxor");

    if (!ret)
    {
        return assert(ret, true, "test_nnxor_load: failed to load network");
    }

    Matrix *test_input = matrix_init(batchsize, 2, NULL);
    Matrix *test_expected = matrix_init(batchsize, 2, NULL);

    // 1 xor 0 = 1
    m_set(test_input, 0, 0, 1);
    m_set(test_input, 0, 1, 0);
    m_set(test_expected, 0, 1, 1);

    // 0 xor 0 = 0
    m_set(test_input, 1, 0, 0);
    m_set(test_input, 1, 1, 0);
    m_set(test_expected, 1, 0, 1);

    Matrix *predictions = nn_forward(network, test_input);

    int failed = 0;

    float p1 = m_get(predictions, 0, 1);
    float p2 = m_get(predictions, 1, 0);

    if (p1 < 0.95)
        failed++;

    if (p2 < 0.95)
        failed++;

    nn_destroy(network);
    matrix_destroy(test_input);
    matrix_destroy(test_expected);
    matrix_destroy(predictions);

    return assert(failed, 0, "test_nnxor_load");
}

int test_cnn()
{

    int batchsize = 4;

    // define the layers
    ConvLayer **conv_layers = malloc(sizeof(ConvLayer));
    conv_layers[0] = conv_layer_init(28, 28, 1, 8, 3, 2, 1, batchsize, leaky_relu, d_leaky_relu, "conv0");
    // conv_layers[1] = conv_layer_init(14, 14, 8, 16, 3, 2, 1, batchsize, sigmoid, d_sigmoid, "conv1");

    FCLayer **fc_layers = malloc(sizeof(FCLayer) * 2);
    fc_layers[0] = fc_layer_init(14 * 14 * 8, 256, batchsize, leaky_relu, d_leaky_relu, "fc0");
    fc_layers[1] = fc_layer_init(256, 10, batchsize, leaky_relu, d_leaky_relu, "fc1");

    ActivationLayer *output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);
    int num_conv_layers = 1;
    int num_fc_layers = 2;

    CNN *network = cnn_init(conv_layers, num_conv_layers, fc_layers, num_fc_layers, output_layer);

    // create a dummy image batch filled with random values
    Matrix4 *input = matrix4_init(batchsize, 1, 28, 28, NULL);


    for (int i = 0; i < batchsize; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            for (int k = 0; k < 28; k++)
            {
                m4_set(input, i, 0, j, k, (rand() % 255) / 255.0);
            }
        }
    }

    Matrix *expected = matrix_init(batchsize, 10, NULL);

    // force category 2
    m_set(expected, 0, 2, 1);
    m_set(expected, 1, 2, 1);
    m_set(expected, 2, 2, 1);
    m_set(expected, 3, 2, 1);
    
    int epochs = 100;
    for (int i = 0; i < epochs; i++)
    {
        double loss = cnn_train_batch(network, input, expected, 0.0001);
        // printf("Epoch %d, loss: %f\n", i, loss);
    }

    // save weights
    cnn_save(network, "tests/out/cnn-test");

    // free the memory
    cnn_destroy(network);
    matrix4_destroy(input);
    matrix_destroy(expected);

    return assert(1, 1, "test_cnn");
}

int test_cnn_load()
{

    int batchsize = 4;

    // define the layers
    ConvLayer **conv_layers = malloc(sizeof(ConvLayer) * 1);
    conv_layers[0] = conv_layer_init(28, 28, 1, 8, 3, 2, 1, batchsize, sigmoid, d_sigmoid, "conv0");
    // conv_layers[1] = conv_layer_init(14, 14, 8, 16, 3, 2, 1, batchsize, sigmoid, d_sigmoid, "conv1");

    FCLayer **fc_layers = malloc(sizeof(FCLayer) * 2);
    fc_layers[0] = fc_layer_init(14 * 14 * 8, 256, batchsize, sigmoid, d_sigmoid, "fc0");
    fc_layers[1] = fc_layer_init(256, 10, batchsize, sigmoid, d_sigmoid, "fc1");

    ActivationLayer *output_layer = activation_layer_init(10, batchsize, softmax, d_softmax);
    int num_conv_layers = 1;
    int num_fc_layers = 2;

    CNN *network = cnn_init(conv_layers, num_conv_layers, fc_layers, num_fc_layers, output_layer);

    bool ret = cnn_load(network, "tests/out/cnn-test");

    if (!ret)
    {
        return assert(ret, true, "test_cnn_load: failed to load network");
    }

    // create a dummy image batch filled with random values
    Matrix4 *input = matrix4_init(batchsize, 1, 28, 28, NULL);

    for (int i = 0; i < batchsize; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            for (int k = 0; k < 28; k++)
            {
                m4_set(input, i, 0, j, k, (rand() % 255) / 255.0);
            }
        }
    }

    // run forward pass
    Matrix *predictions = cnn_forward(network, input);

    // free the memory
    cnn_destroy(network);
    matrix4_destroy(input);
    matrix_destroy(predictions);

    return assert(1, 1, "test_cnn_load");
}