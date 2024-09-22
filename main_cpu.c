#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bpsk_modulation_cpu(const int* input_bits, float* modulated_signal, int num_bits) {
    for (int i = 0; i < num_bits; i++) {
        // BPSK: 0 -> +1, 1 -> -1
        modulated_signal[i] = (input_bits[i] == 0) ? 1.0f : -1.0f;
    }
}

int main() {
    int num_bits = 1000000;  // Number of bits to modulate
    int *input_bits = (int*)malloc(sizeof(int) * num_bits);
    float *modulated_signal = (float*)malloc(sizeof(float) * num_bits);

    // Randomly generate input bits
    srand(time(NULL));
    for (int i = 0; i < num_bits; i++) {
        input_bits[i] = rand() % 2;
    }

    // Perform BPSK modulation on CPU
    clock_t start = clock();
    bpsk_modulation_cpu(input_bits, modulated_signal, num_bits);
    clock_t end = clock();

    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("CPU Time: %f seconds\n", cpu_time);

    free(input_bits);
    free(modulated_signal);

    return 0;
}
