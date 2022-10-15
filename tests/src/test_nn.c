
#include "../include/test_nn.h"

int test_nnxor()
{
    NNXor *network = nnxor_init(4, false);
    Matrix *input = matrix_init(4, 2, NULL);
    Matrix *expected = matrix_init(4, 2, NULL);

    // shape of input: (4, 2) (batchsize, size)
    // shape of expected: (4, 2) (batchsize, size)

    // 0 xor 0 = 0
    matrix_set(input, 0, 0, 0); 
    matrix_set(input, 0, 1, 0);
    matrix_set(expected, 0, 0, 1);

    // 0 xor 1 = 1
    matrix_set(input, 1, 0, 0);
    matrix_set(input, 1, 1, 1);
    matrix_set(expected, 1, 1, 1);

    // 1 xor 0 = 1
    matrix_set(input, 2, 0, 1);
    matrix_set(input, 2, 1, 0);
    matrix_set(expected, 2, 1, 1);

    // 1 xor 1 = 0
    matrix_set(input, 3, 0, 1);
    matrix_set(input, 3, 1, 1);
    matrix_set(expected, 3, 0, 1);

    for (int i = 0; i < 500; i++)
    {
        nnxor_train_batch(network, input, expected, 0.001);
    }

    Matrix *predictions = nnxor_forward(network, input);

    int failed = 0;
    failed = matrix_get(predictions, 0, 0) > 0.9 ? 0 : 1;
    failed = matrix_get(predictions, 1, 1) > 0.9 ? 0 : 1;
    failed = matrix_get(predictions, 2, 1) > 0.9 ? 0 : 1;
    failed = matrix_get(predictions, 3, 0) > 0.9 ? 0 : 1;

    nnxor_destroy(network);
    matrix_destroy(input);
    matrix_destroy(expected);
    matrix_destroy(predictions);

    return print_test(failed == 0, true, "test_nnxor");
}
