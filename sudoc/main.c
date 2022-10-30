#include <stdlib.h>
#include "include/cv.h"
#include "include/solver.h"
#include "include/neuralnet.h"
#include "../tests/include/test_cv.h"

int main(int argc, char **argv)
{
    mkdir("out", 0777);

    if (argc < 3)
        errx(EXIT_FAILURE, "Usage: demo-type demo-name demo-args");

    if (strcmp(argv[1], "cv") == 0)
    {
        if (strcmp(argv[2], "gaussian") == 0)
        {
            test_s1_cv_gaussian_blur();
        }
        else if (strcmp(argv[2], "sobel") == 0)
        {
            test_s1_cv_sobel();
        }
        else if (strcmp(argv[2], "canny") == 0)
        {
            test_s1_cv_canny();
        }
        else if (strcmp(argv[2], "threshold") == 0)
        {
            test_s1_cv_adaptive_threshold();
        }
        else if (strcmp(argv[2], "houghlines") == 0)
        {
            test_s1_cv_hough_lines();
        }
        else if (strcmp(argv[2], "rotate") == 0)
        {
            if (argc < 4)
                errx(EXIT_FAILURE, "Usage: cv rotate rotation-angle");
            test_s1_cv_rotate(atoi(argv[3]));
        }
        else if (strcmp(argv[2], "boxes") == 0)
        {
            test_s1_cv_save_boxes();
        }
        else
        {
            errx(EXIT_FAILURE, "Unknown demo name");
        }
    }

    else if (strcmp(argv[1], "nn") == 0)
    {
        if (strcmp(argv[2], "train-xor") == 0)
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

            int num_epochs = 10000;
            for (int i = 0; i < num_epochs; i++)
            {
                float loss = nn_train_batch(network, input, expected, 0.001);
                printf("\rLoss: %f, epoch %i/%i", loss, i + 1, num_epochs);
                fflush(stdout);
            }
            printf("\n");

            matrix_destroy(input);
            matrix_destroy(expected);

            // save weights
            char *path = "out/nnxor";
            nn_save(network, path);

            printf("Saved weights to %s\n", path);

            nn_destroy(network);
        }

        if (strcmp(argv[2], "test-xor") == 0)
        {
            init_rand();

            int batchsize = 1;

            // define the layers
            FCLayer **fc_layers = malloc(sizeof(FCLayer));
            fc_layers[0] = fc_layer_init(2, 32, batchsize, leaky_relu, d_leaky_relu, "fc0");
            fc_layers[1] = fc_layer_init(32, 32, batchsize, leaky_relu, d_leaky_relu, "fc1");
            fc_layers[2] = fc_layer_init(32, 2, batchsize, leaky_relu, d_leaky_relu, "fc2");

            ActivationLayer *output_layer = activation_layer_init(2, batchsize, softmax, d_softmax);
            int num_fc_layers = 3;

            NN *network = nn_init(fc_layers, num_fc_layers, output_layer);
            Matrix *input = matrix_init(batchsize, 2, NULL);

            // load weights
            char *path = "out/nnxor";
            int loaded = nn_load(network, path);

            if (loaded)
            {
                printf("Loaded weights from %s\n", path);
            }
            else
            {
                printf("could not load weights from %s\n", path);
            }

            char *exit_word = "exit";
            char *str = malloc(100);
            while (strcmp(str, exit_word) != 0)
            {
                printf("Enter two numbers (0 or 1) separated by a space: ");

                int reta = scanf("%s", str);
                if (strcmp(str, exit_word) == 0)
                    break;
                if (reta != 1)
                    errx(EXIT_FAILURE, "scanf failed");

                int a = atoi(str);

                int retb = scanf("%s", str);
                if (retb != 1)
                    errx(EXIT_FAILURE, "scanf failed");

                int b = atoi(str);

                m_set(input, 0, 0, (float)a);
                m_set(input, 0, 1, (float)b);

                Matrix *output = nn_forward(network, input);

                printf("Raw output: %f, %f\n", m_get(output, 0, 0), m_get(output, 0, 1));

                // get the maximum arg
                float r0 = m_get(output, 0, 1);
                float r1 = m_get(output, 0, 0);
                int max_arg = r0 > r1 ? 1 : 0;

                printf("Result: %i xor %i = %i\n", a, b, max_arg);

                matrix_destroy(output);
            }

            free(str);
            matrix_destroy(input);
            nn_destroy(network);
        }
        else
        {
            errx(EXIT_FAILURE, "Unknown demo name");
        }
    }

    return EXIT_SUCCESS;
}
