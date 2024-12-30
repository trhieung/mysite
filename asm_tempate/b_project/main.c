#include <stdio.h>

// void __asm_action() {
//     void* stack_ptr;
//     __asm__ __volatile__(
//         "sub $12, %%rsp\n"              // Allocate space on the stack (12 bytes for alignment)
//         "movb $0x30, (%%rsp)\n"         // Write '0' at the top of the stack
//         "movb $0x31, 1(%%rsp)\n"        // Write '1' at offset 1
//         "movb $0x32, 2(%%rsp)\n"        // Write '2' at offset 2
//         "movb $0x33, 3(%%rsp)\n"        // Write '3' at offset 3
//         "movb $0x34, 4(%%rsp)\n"        // Write '4' at offset 4
//         "movb $0x35, 5(%%rsp)\n"        // Write '5' at offset 5
//         "movb $0x36, 6(%%rsp)\n"        // Write '6' at offset 6
//         "movb $0x37, 7(%%rsp)\n"        // Write '7' at offset 7
//         "movb $0x38, 8(%%rsp)\n"        // Write '8' at offset 8
//         "movb $0x39, 9(%%rsp)\n"        // Write '9' at offset 9
//         "movb $0x21, 10(%%rsp)\n"       // Write '!' at offset 10
//         "movb $0x0A, 11(%%rsp)\n"       // Write newline '\n' at offset 11

//         "movq %%rsp, %0\n"              // Store stack pointer in stack_ptr

//         "movl $1, %%eax\n"              // Syscall number for sys_write
//         "movl $1, %%edi\n"              // File descriptor 1 (stdout)
//         "movq %%rsp, %%rsi\n"           // Address of the data (stack pointer)
//         "movl $12, %%edx\n"             // Length of data to write (12 bytes)
//         "syscall\n"                     // Make the system call

//         "add $12, %%rsp\n"              // Restore stack pointer
//         : "=r"(stack_ptr)               // Output: store the stack pointer in stack_ptr
//         :                               // No input operands
//         : "memory", "rax", "rdi", "rsi", "rdx" // Clobbered registers
//     );

//     printf("Stack address: %p\n", stack_ptr);
// }
void __asm_action() {
    __asm__ (
        "sub $12, %%rsp\n"              // Allocate space on the stack (12 bytes for alignment)
        "movb $0x30, (%%rsp)\n"         // Write '0' at the top of the stack
        "movb $0x31, 1(%%rsp)\n"        // Write '1' at offset 1
        "movb $0x32, 2(%%rsp)\n"        // Write '2' at offset 2
        "movb $0x33, 3(%%rsp)\n"        // Write '3' at offset 3
        "movb $0x34, 4(%%rsp)\n"        // Write '4' at offset 4
        "movb $0x35, 5(%%rsp)\n"        // Write '5' at offset 5
        "movb $0x36, 6(%%rsp)\n"        // Write '6' at offset 6
        "movb $0x37, 7(%%rsp)\n"        // Write '7' at offset 7
        "movb $0x38, 8(%%rsp)\n"        // Write '8' at offset 8
        "movb $0x39, 9(%%rsp)\n"        // Write '9' at offset 9
        "movb $0x21, 10(%%rsp)\n"       // Write '!' at offset 10
        "movb $0x0A, 11(%%rsp)\n"       // Write newline '\n' at offset 11


        "movl $1, %%eax\n"              // Syscall number for sys_write
        "movl $1, %%edi\n"              // File descriptor 1 (stdout)
        "movq %%rsp, %%rsi\n"           // Address of the data (stack pointer)
        "movl $12, %%edx\n"             // Length of data to write (12 bytes)
        "syscall\n"                     // Make the system call

        "add $12, %%rsp\n"              // Restore stack pointer
        :                               // No input operands
        :
        : "rax", "rdi", "rsi", "rdx" // Clobbered registers
    );
}

int main() {
    __asm_action();  // Call the assembly function to print the character
    return 0;
}

/* After deassem
xxd main.o > main_o.txt && \
xxd main > main.txt && \
objdump -d main.o


f30f 1efa e977 ffff fff3 0f1e fa55 4889
e548 83ec 02c6 0424 21c6 4424 010a b801
0000 00bf 0100 0000 4889 e6ba 0200 0000
0f05 4883 c402 905d c3


f30f 1efa 
--
e977 ffff ff
f3 0f1e fa
--
55 
4889 e5
48 83ec 02
c6 0424 21
c6 4424 010a 
b801 0000 00
bf 0100 0000 
4889 e6
ba 0200 0000
0f05 
4883 c402
-- end functions return to main 
90
5d 
c3

I call the buff only instead of define in my program note that it requre value load from stack for passing?
*/