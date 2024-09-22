The project can be compiled for both CPU and GPU using the make command. Below are the available targets.

## CPU Build

To compile the CPU version of the program, run:

bash
make cpu


This will generate an executable named cpu_program.

 ## GPU Build

To compile the GPU version of the program, run:

bash
make gpu


This will generate an executable named gpu_program.

## Clean

To clean up the compiled binaries, run:

bash
make clean


This will remove cpu_program, gpu_program, and any other generated files.

## Running the Programs

Once compiled, you can run the programs directly from the command line.

## CPU Program

To run the CPU version:

bash
./cpu_program


This will output the performance metrics for the CPU implementation, including memory allocation time, modulation time, and total execution time.

## GPU Program

To run the GPU version:

bash
./gpu_program


This will output the performance metrics for the GPU implementation, including kernel execution time, memory write time, and memory read time.

## Profiling with OpenCL

The GPU program uses OpenCL's eventbased profiling to report detailed timing information for:

 Kernel execution time: Time taken to execute the BPSK modulation on the GPU.
 Memory write time: Time taken to transfer data from the CPU to the GPU.
 Memory read time: Time taken to transfer results from the GPU back to the CPU.

These metrics are printed in milliseconds after running the gpu_program.

## Example Output

### CPU Program Output

Memory Allocation Time: 0.000051 seconds
BPSK Modulation Time (CPU): 0.005181 seconds
Total Execution Time: 0.044354 seconds


### GPU Program Output

Kernel execution time: 0.455363 ms
Memory write time: 0.290568 ms
Memory read time: 1.438395 ms
