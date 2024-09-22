#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "device.h"
#include "kernel.h"

#define KERNEL_PATH "kernel.cl"

void checkError(cl_int err, const char* op) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error during operation '%s': %d\n", op, err);
        exit(1);
    }
}

char* readKernelSource(const char* kernelPath) {
    FILE *fp;
    long length;
    char *source;

    fp = fopen(kernelPath, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel file.\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    source = (char*)malloc(length + 1);
    source[length] = '\0';

    fread(source, sizeof(char), length, fp);
    fclose(fp);

    return source;
}

int main() {
    int num_bits = 1000000;
    int *input_bits = (int*)malloc(sizeof(int) * num_bits);
    float *modulated_signal = (float*)malloc(sizeof(float) * num_bits);

    // Randomly generate input bits
    srand(time(NULL));
    for (int i = 0; i < num_bits; i++) {
        input_bits[i] = rand() % 2;
    }

    // OpenCL Setup
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem inputBuffer, outputBuffer;
    cl_int err;

    // Get platform and device information
    err = clGetPlatformIDs(1, &platform_id, NULL);
    checkError(err, "clGetPlatformIDs");

    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    checkError(err, "clGetDeviceIDs");

    // Create OpenCL context and command queue
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    queue = clCreateCommandQueue(context, device_id, 0, &err);
    checkError(err, "clCreateCommandQueue");

    // Create buffers
    inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * num_bits, input_bits, &err);
    checkError(err, "clCreateBuffer(input)");

    outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * num_bits, NULL, &err);
    checkError(err, "clCreateBuffer(output)");

    // Read the kernel source from file using KERNEL_PATH
    char *kernelSource = readKernelSource(KERNEL_PATH);

    // Create the program from source
    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);
    checkError(err, "clCreateProgramWithSource");

    // Build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    checkError(err, "clBuildProgram");

    // Create the OpenCL kernel
    kernel = clCreateKernel(program, "bpsk_modulation", &err);
    checkError(err, "clCreateKernel");

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    checkError(err, "clSetKernelArg(input)");

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    checkError(err, "clSetKernelArg(output)");

    err = clSetKernelArg(kernel, 2, sizeof(int), &num_bits);
    checkError(err, "clSetKernelArg(num_bits)");

    // Execute the kernel
    size_t globalSize = num_bits;
    clock_t start = clock();
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");

    // Wait for the command queue to complete
    clFinish(queue);
    clock_t end = clock();

    double gpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("GPU Time: %f seconds\n", gpu_time);

    // Read the output buffer back to host
    err = clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, sizeof(float) * num_bits, modulated_signal, 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer");

    // Cleanup
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(input_bits);
    free(modulated_signal);
    free(kernelSource);

    return 0;
}
