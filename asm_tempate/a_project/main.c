#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 32-bit register structure
typedef struct __registers_32b
{
    uint32_t eax; // Accumulator  -   Used for arithmetic and return values
    uint32_t ebx; // Base         -   Used for data manipulation and memory address
    uint32_t ecx; // Count        -   Often used as a counter for loops and string operations
    uint32_t edx; // Data         -   Used in multiplication, division, and some I/O operations
} reg_32b;

// 64-bit register structure
typedef struct __registers_64b
{
    uint64_t rax; // Accumulator  -   Used for arithmetic and return values
    uint64_t rbx; // Base         -   Used for data manipulation and memory address
    uint64_t rcx; // Count        -   Often used as a counter for loops and string operations
    uint64_t rdx; // Data         -   Used in multiplication, division, and some I/O operations
} reg_64b;

// Define a generic register type based on the target architecture
#if defined(__x86_64__) // 64-bit architecture
typedef reg_64b reg;
#else // 32-bit architecture (e.g., x86)
typedef reg_32b reg;
#endif

void cpuid(reg *registers, const bool log)
{

    // Inline assembly to execute the CPUID instruction
#if defined(__x86_64__) // 64-bit architecture
    // Set RAX to 0 to get the vendor string (CPUID function 0)
    registers->rax = 0;
    __asm__(
        "CPUID"
        /* All outputs (rax/ebx, rcx/edx depending on architecture) */
        : "=a"(registers->rax), "=b"(registers->rbx), "=c"(registers->rcx), "=d"(registers->rdx)
        /* Input (eax) */
        : "a"(registers->rax));
#else // 32-bit architecture (e.g., x86)
    // Set EAX to 0 to get the vendor string (CPUID function 0)
    registers->eax = 0;
    __asm__(
        "CPUID"
        /* All outputs (eax/ebx, rcx/edx depending on architecture) */
        : "=a"(registers->eax), "=b"(registers->ebx), "=c"(registers->ecx), "=d"(registers->edx)
        /* Input (eax) */
        : "a"(registers->eax));
#endif

    if (log)
    {
        printf("CPUID results:\n");
#if defined(__x86_64__)                       // 64-bit architecture
        printf("RAX: %lu\n", registers->rax); // Use %llu for printing uint64_t values
        printf("RBX: %lu\n", registers->rbx);
        printf("RCX: %lu\n", registers->rcx);
        printf("RDX: %lu\n", registers->rdx);
#else // 32-bit architecture (e.g., x86)
        printf("EAX: %u\n", registers->eax); // Use %u for printing uint32_t values
        printf("EBX: %u\n", registers->ebx);
        printf("ECX: %u\n", registers->ecx);
        printf("EDX: %u\n", registers->edx);
#endif
    }
}

int _addCheck(int *a, int *b, reg *registers)
{
    int result;

    // Inline assembly to add values of a and b
    __asm__(
        "movl (%[a]), %%eax;"     // Load the value of 'a' (dereference pointer) into eax - which for 32 bits value
        "addl (%[b]), %%eax;"     // Add the value of 'b' (dereference pointer) to eax
        "movl %%eax, %[result];"  // Store the result of the addition in 'result'
        : [result] "=r"(result)  // Output result
        : [a] "r"(a), [b] "r"(b) // Inputs: pointers to a and b
        : "%eax"                 // Clobbered registers
    );

    return result; // Return the sum of *a and *b
}

int main(void)
{
    // reg registers = {0}; // Initialize registers with eax = 0 for CPUID

    // cpuid(&registers, true); // Log the CPUID results

    // int a = 10;
    // int b = 5;

    // int result = _addCheck(&a, &b, &registers); // Pass pointers to a and b
    // printf("Addition result: %d\n", result);
    // Print the address of the printf function
    printf("Address of printf function: %p\n", (void*)printf);

    return 0;
}
