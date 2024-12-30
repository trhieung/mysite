CC = gcc
CFLAGS = -no-pie -Wall -g -m64
LDFLAGS =

SRC = main.c
ASM_SRC = asm_action.s
OBJ = main.o asm_action.o
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

asm_action.o: $(ASM_SRC)
	$(CC) $(CFLAGS) -c $(ASM_SRC)

clean:
	rm -f $(OBJ) $(EXEC)
