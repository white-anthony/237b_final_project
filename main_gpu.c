#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "device.h"
#include "kernel.h"

#define KERNEL_PATH "kernel.cl"

// Function to check and handle OpenCL errors
void checkError(cl_int err, const char* op) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error during operation '%s': %d\n", op, err);
        exit(1);
    }
}

// Function to read the kernel source code
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

    // Create OpenCL context and command queue with profiling enabled
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    // Enable profiling in the command queue
    queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
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

    // Profiling variables
    cl_event kernel_event, read_event, write_event;
    cl_ulong start, end;
    double kernel_exec_time, read_time, write_time;

    // Enqueue write operation to input buffer (profiling event created)
    err = clEnqueueWriteBuffer(queue, inputBuffer, CL_TRUE, 0, sizeof(int) * num_bits, input_bits, 0, NULL, &write_event);
    checkError(err, "clEnqueueWriteBuffer");

    // Enqueue the kernel (profiling event created)
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, &kernel_event);
    checkError(err, "clEnqueueNDRangeKernel");

    // Enqueue read operation from output buffer (profiling event created)
    err = clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, sizeof(float) * num_bits, modulated_signal, 0, NULL, &read_event);
    checkError(err, "clEnqueueReadBuffer");

    // Wait for all commands to complete
    clFinish(queue);

    // Calculate profiling data
    clGetEventProfilingInfo(write_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(write_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    write_time = (end - start) / 1000000.0; // Convert to milliseconds

    clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernel_exec_time = (end - start) / 1000000.0; // Convert to milliseconds

    clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    read_time = (end - start) / 1000000.0; // Convert to milliseconds

    // Print profiling results
    printf("Kernel execution time: %f ms\n", kernel_exec_time);
    printf("Memory write time: %f ms\n", write_time);
    printf("Memory read time: %f ms\n", read_time);

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
