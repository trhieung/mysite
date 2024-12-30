# Compiler settings
CC = gcc
CFLAGS = -m64 -march=native -Wall

# Target executable name
TARGET = cpuid_example

# Source files
SRC = main.c

# Object files (generated from the source files)
OBJ = $(SRC:.c=.o)

# Default target to build the program
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Rule to compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets (these don't generate actual files)
.PHONY: all clean
