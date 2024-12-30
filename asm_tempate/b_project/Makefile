# Compiler and linker
CC = gcc
CFLAGS = -Wall -g  # Enable all warnings and debugging info
LDFLAGS = 

# Source and output
SRC = main.c
OBJ = main.o
OUT = main

# Default target: compile and link
all: $(OUT)

# Rule to compile the .c file to .o file
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Rule to link the .o file to create the executable
$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(OUT)

# Clean up the compiled files
clean:
	rm -f $(OBJ) $(OUT)

# Rule to rebuild everything
rebuild: clean all

.PHONY: all clean rebuild
