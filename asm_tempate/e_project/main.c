#include <stdio.h>

extern void __asm_action(); // Declare the external assembly function

int main() {
    __asm_action(); // Call the external function
    return 0;
}

/* Deassem command
xxd main.o > main_o.txt && \
xxd main > main.txt && \
objdump -d main.o

*/