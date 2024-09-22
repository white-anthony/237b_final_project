# Compiler and flags
CC       = gcc
CFLAGS   = -g -Wall
INCFLAGS := -I./helper_lib
LDFLAGS  := ./helper_lib/helper_lib.a -lm

# Platform-specific flags
ifeq ($(shell uname -o), Darwin)
	LDFLAGS += -framework OpenCL
else ifeq ($(shell uname -o), GNU/Linux) # Assumes NVIDIA GPU
	LDFLAGS  += -L/usr/local/cuda/lib64 -lOpenCL
	INCFLAGS += -I/usr/local/cuda/include
else # Android
	LDFLAGS += -lOpenCL
endif

# Files
CPU_SRC = main_cpu.c
GPU_SRC = main_gpu.c
KERNEL = kernel.cl

# Output binaries
CPU_BIN = cpu_program
GPU_BIN = gpu_program

# Default target (no build)
all:
	@echo "Use 'make cpu' or 'make gpu' to build the appropriate version."

# Build helper library if it doesn't exist
./helper_lib/helper_lib.a:
	$(MAKE) -C ./helper_lib

# CPU build
cpu: ./helper_lib/helper_lib.a $(CPU_SRC)
	$(CC) $(CFLAGS) -o $(CPU_BIN) $(CPU_SRC) $(INCFLAGS) $(LDFLAGS)
	@echo "CPU version compiled successfully!"

# GPU build
gpu: ./helper_lib/helper_lib.a $(GPU_SRC) $(KERNEL)
	$(CC) $(CFLAGS) -o $(GPU_BIN) $(GPU_SRC) $(INCFLAGS) $(LDFLAGS)
	@echo "GPU version compiled successfully!"

# Clean up generated files
clean:
	rm -f $(CPU_BIN) $(GPU_BIN)
	@echo "Cleaned up binaries!"
