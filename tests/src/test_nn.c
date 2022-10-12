
#include "../include/test_nn.h"

int test_nnxor()
{
    NNXor *network = nnxor_init(4, false);
    Matrix *input = matrix_init(2, 4, NULL);
    Matrix *expected = matrix_init(2, 4, NULL);

    // shape of input: (2, 4) (size, batchsize)
    // shape of expected: (2, 4) (size, batchsize)

    // 0 xor 0 = 0
    matrix_set(input, 0, 0, 0); 
    matrix_set(input, 1, 0, 0);
    matrix_set(expected, 0, 0, 1);

    // 0 xor 1 = 1
    matrix_set(input, 0, 1, 0);
    matrix_set(input, 1, 1, 1);
    matrix_set(expected, 1, 1, 1);

    // 1 xor 0 = 1
    matrix_set(input, 0, 2, 1);
    matrix_set(input, 1, 2, 0);
    matrix_set(expected, 1, 2, 1);

    // 1 xor 1 = 0
    matrix_set(input, 0, 3, 1);
    matrix_set(input, 1, 3, 1);
    matrix_set(expected, 0, 3, 1);

    for (int i = 0; i < 1000; i++)
    {
        float loss = nnxor_train_batch(network, input, expected, 0.1);
        printf("Loss: %f, Iteration: %d", loss, i);
    }

    nnxor_destroy(network);

    return 1;
}
