
#include "../include/test_nn.h"

int test_nnxor()
{

    int batchsize = 4;
    bool load_weights = false;
    
    
    // define the layers
    FCLayer **fc_layers = malloc(sizeof(FCLayer));
    fc_layers[0] = fc_layer_init(2, 32, batchsize, leaky_relu, d_leaky_relu, load_weights, "fc1");
    fc_layers[1] = fc_layer_init(32, 32, batchsize, leaky_relu, d_leaky_relu, load_weights, "fc2");
    fc_layers[2] = fc_layer_init(32, 2, batchsize, leaky_relu, d_leaky_relu, load_weights, "fc3");
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

    for (int i = 0; i < 500; i++)
    {
        nn_train_batch(network, input, expected, 0.001);
    }

    matrix_destroy(input);
    matrix_destroy(expected);

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


    Matrix *predictions = nn_forward(network, test_input);

    int failed = 0;
    failed = m_get(predictions, 0, 1) > 0.9 ? 0 : 1;
    failed = m_get(predictions, 1, 0) > 0.9 ? 0 : 1;
    failed = m_get(predictions, 2, 0) > 0.9 ? 0 : 1;
    failed = m_get(predictions, 3, 1) > 0.9 ? 0 : 1;

    nn_destroy(network);
    matrix_destroy(test_input);
    matrix_destroy(test_expected);
    matrix_destroy(predictions);

    return print_test(failed == 0, true, "test_nnxor");
}
