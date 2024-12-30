#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define LENGTH 10

int main() {
    int a[LENGTH], b[LENGTH], result[LENGTH];
    for (int i = 0; i < LENGTH; i++) {
        a[i] = i;
        b[i] = a[i] + 1;
    }

    // 1. Initialize OpenCL
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;

    size_t global_work_size = LENGTH;
    cl_int err;

    // 2. Get platform and device information
    clGetPlatformIDs(1, &platform_id, NULL);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    // 3. Create context and command queue
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device_id, 0, &err);

    // 4. Load kernel source
    const char *kernel_src =
        "__kernel void vector_add(__global int *a, __global int *b, __global int *result, const int length) {"
        "    int id = get_global_id(0);"
        "    if (id < length) {"
        "        result[id] = a[id] + b[id];"
        "    }"
        "}";

    program = clCreateProgramWithSource(context, 1, &kernel_src, NULL, &err);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "vector_add", &err);

    // 5. Create buffers
    cl_mem buffer_a = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * LENGTH, NULL, &err);
    cl_mem buffer_b = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * LENGTH, NULL, &err);
    cl_mem buffer_result = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * LENGTH, NULL, &err);

    // 6. Write data to buffers
    clEnqueueWriteBuffer(queue, buffer_a, CL_TRUE, 0, sizeof(int) * LENGTH, a, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, buffer_b, CL_TRUE, 0, sizeof(int) * LENGTH, b, 0, NULL, NULL);

    // 7. Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_a);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_b);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_result);
    clSetKernelArg(kernel, 3, sizeof(int), &LENGTH);

    // 8. Execute the kernel
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

    // 9. Read the result
    clEnqueueReadBuffer(queue, buffer_result, CL_TRUE, 0, sizeof(int) * LENGTH, result, 0, NULL, NULL);

    // 10. Print the result
    printf("Result:\n");
    for (int i = 0; i < LENGTH; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // 11. Clean up
    clReleaseMemObject(buffer_a);
    clReleaseMemObject(buffer_b);
    clReleaseMemObject(buffer_result);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
