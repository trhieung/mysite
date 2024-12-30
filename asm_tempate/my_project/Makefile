# Compiler and assembler settings
CC = gcc
AS = as
LD = gcc
CFLAGS = -Wall -g
ASMFLAGS = --64  # 64-bit assembly (x86-64)
LDFLAGS = -no-pie  # Disable position-independent executable, if needed

# File names
ASM_SRC = add.asm
OBJ = add.o main.o
OUT = main

# Default target: build the program
all: $(OUT)

# Rule for compiling C files
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# Rule for assembling the assembly file
add.o: $(ASM_SRC)
	$(AS) $(ASMFLAGS) -o add.o add.asm

# Link the object files into an executable
$(OUT): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(OUT)

# Clean up object files and the executable
clean:
	rm -f $(OBJ) $(OUT)
