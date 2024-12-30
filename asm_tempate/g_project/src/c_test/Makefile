# Compiler and flags
CC = gcc
CFLAGS = -no-pie -Wall -g -m64
LDFLAGS = 

# Directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
ASM_PATH = ../../payloads/asm/*.s

# Source files
SRC = main.c
ASM_SRC = $(ASM_PATH)
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/linux_64__bind_tcp.o
EXEC = $(BUILD_DIR)/main

# Create build directory and object directory if not exist
$(shell mkdir -p $(OBJ_DIR))

# Default target
all: $(EXEC)

# Linking the final executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Compilation of C files
$(OBJ_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

# Compilation of ASM files
$(OBJ_DIR)/%.o: $(ASM_PATH)
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

# Clean up object files and executable
clean:
	rm -rf $(BUILD_DIR)


