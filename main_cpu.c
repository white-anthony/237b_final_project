#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function for BPSK modulation on CPU
void bpsk_modulation_cpu(const int* input_bits, float* modulated_signal, int num_bits) {
    for (int i = 0; i < num_bits; i++) {
        // BPSK: 0 -> +1, 1 -> -1
        modulated_signal[i] = (input_bits[i] == 0) ? 1.0f : -1.0f;
    }
}

int main() {
    int num_bits = 1000000;  // Number of bits to modulate
    clock_t start, end;
    double allocation_time, modulation_time, total_time;

    // Start timing for the total execution
    start = clock();

    // Start timing for memory allocation
    clock_t alloc_start = clock();
    int *input_bits = (int*)malloc(sizeof(int) * num_bits);
    float *modulated_signal = (float*)malloc(sizeof(float) * num_bits);
    clock_t alloc_end = clock();
    allocation_time = ((double)(alloc_end - alloc_start)) / CLOCKS_PER_SEC;

    // Randomly generate input bits
    srand(time(NULL));
    for (int i = 0; i < num_bits; i++) {
        input_bits[i] = rand() % 2;
    }

    // Start timing for the modulation process
    clock_t mod_start = clock();
    bpsk_modulation_cpu(input_bits, modulated_signal, num_bits);
    clock_t mod_end = clock();
    modulation_time = ((double)(mod_end - mod_start)) / CLOCKS_PER_SEC;

    // End timing for the total execution
    end = clock();
    total_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Print results
    printf("Memory Allocation Time: %f seconds\n", allocation_time);
    printf("BPSK Modulation Time (CPU): %f seconds\n", modulation_time);
    printf("Total Execution Time: %f seconds\n", total_time);

    // Free allocated memory
    free(input_bits);
    free(modulated_signal);

    return 0;
}
