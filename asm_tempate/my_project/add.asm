.section .data
    fmt: .asciz "Value of rdi: %d\n"    # Format string for printing

.section .text
.globl add, add_four

# Function: add
# Adds two integers (passed in %rdi and %rsi) and returns the result in %rax.
add:
    mov %rdi, %rax      # Move first argument (in %rdi) to %rax
    add %rsi, %rax      # Add second argument (in %rsi) to %rax
    ret                  # Return

# Function: add_four
# Adds four integers and prints the value of the first argument (%rdi):
# - First two integers in %rdi and %rsi
# - Next two integers from the stack (rsp points to them)
# Returns the result in %rax.
add_four:
    # Print the value of %rdi (first argument)
    mov rdi, fmt         # Load the address of the format string into %rdi
    mov rsi, %rdi        # Move the value of %rdi (to print) into %rsi (fix)
    xor rax, rax         # Clear rax (no floating point arguments)
    call printf          # Call printf

    # Continue with adding the four numbers
    mov %rdi, %rax       # Copy first argument (a) to %rax
    add %rsi, %rax       # Add second argument (b) to %rax
    add %rdx, %rax       # Add third argument (c) to %rax
    add %rcx, %rax       # Add fourth argument (d) to %rax

    ret                  # Return with the result in %rax
