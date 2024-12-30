#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define VECTOR_SIZE 10

// OpenCL kernel to add two vectors
const char *kernel_source = 
"__kernel void vec_add(__global const int* A, __global const int* B, __global int* C) {  \n"
"    int i = get_global_id(0);  \n"
"    C[i] = A[i] + B[i];  \n"
"} \n";

int main() {
    // Sample input vectors
    int A[VECTOR_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int B[VECTOR_SIZE] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int C[VECTOR_SIZE];  // Resultant vector

    cl_int err;
    cl_platform_id platform_id;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    cl_mem buffer_A, buffer_B, buffer_C;

    // Step 1: Get available OpenCL platforms
    cl_uint num_platforms;
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    if (err != CL_SUCCESS || num_platforms == 0) {
        printf("Error: No OpenCL platforms found\n");
        return EXIT_FAILURE;
    }

    cl_platform_id platforms[num_platforms];
    err = clGetPlatformIDs(num_platforms, platforms, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Unable to get platform IDs\n");
        return EXIT_FAILURE;
    }

    platform_id = platforms[0]; // Select the first available platform

    // Step 2: Get available OpenCL devices
    cl_uint num_devices;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
    if (err != CL_SUCCESS || num_devices == 0) {
        printf("Error: No OpenCL devices found for the selected platform\n");
        return EXIT_FAILURE;
    }

    cl_device_id devices[num_devices];
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Unable to get device IDs\n");
        return EXIT_FAILURE;
    }

    device_id = devices[0]; // Select the first available device

    // Step 3: Create an OpenCL context
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create OpenCL context\n");
        return EXIT_FAILURE;
    }

    // Step 4: Create a command queue using clCreateCommandQueueWithProperties
    cl_queue_properties queue_properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, 0};
    queue = clCreateCommandQueueWithProperties(context, device_id, queue_properties, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create command queue\n");
        return EXIT_FAILURE;
    }

    // Step 5: Create buffers
    buffer_A = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * VECTOR_SIZE, NULL, &err);
    buffer_B = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * VECTOR_SIZE, NULL, &err);
    buffer_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * VECTOR_SIZE, NULL, &err);

    if (err != CL_SUCCESS) {
        printf("Error: Failed to create buffers\n");
        return EXIT_FAILURE;
    }

    // Step 6: Write input data to buffers
    err = clEnqueueWriteBuffer(queue, buffer_A, CL_TRUE, 0, sizeof(int) * VECTOR_SIZE, A, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(queue, buffer_B, CL_TRUE, 0, sizeof(int) * VECTOR_SIZE, B, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to write data to buffers\n");
        return EXIT_FAILURE;
    }

    // Step 7: Create program and build
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create program\n");
        return EXIT_FAILURE;
    }

    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to build program\n");
        return EXIT_FAILURE;
    }

    // Step 8: Create kernel
    cl_kernel kernel = clCreateKernel(program, "vec_add", &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create kernel\n");
        return EXIT_FAILURE;
    }

    // Step 9: Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_A);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_B);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_C);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to set kernel arguments\n");
        return EXIT_FAILURE;
    }

    // Step 10: Execute the kernel
    size_t global_work_size[1] = {VECTOR_SIZE};
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to enqueue kernel\n");
        return EXIT_FAILURE;
    }

    // Step 11: Read the result
    err = clEnqueueReadBuffer(queue, buffer_C, CL_TRUE, 0, sizeof(int) * VECTOR_SIZE, C, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to read buffer\n");
        return EXIT_FAILURE;
    }

    // Step 12: Output the result
    printf("Result of vector addition:\n");
    for (int i = 0; i < VECTOR_SIZE; i++) {
        printf("%d + %d = %d\n", A[i], B[i], C[i]);
    }

    // Step 13: Clean up
    clReleaseMemObject(buffer_A);
    clReleaseMemObject(buffer_B);
    clReleaseMemObject(buffer_C);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
