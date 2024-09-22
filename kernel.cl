__kernel void bpsk_modulation(__global const int* input_bits, __global float* modulated_signal, int num_bits) {
    int i = get_global_id(0); // Each thread processes one bit
    
    if (i < num_bits) {
        // BPSK modulation: 0 -> +1, 1 -> -1
        modulated_signal[i] = (input_bits[i] == 0) ? 1.0f : -1.0f;
    }
}
