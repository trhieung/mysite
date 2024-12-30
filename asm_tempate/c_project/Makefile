CC = gcc
CFLAGS = -O2 -Wall -std=c11 -DCL_TARGET_OPENCL_VERSION=220
LDFLAGS = -lOpenCL

TARGET = vector_add
SRC = vector_add.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
