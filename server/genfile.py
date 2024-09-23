import sys
import os
import random
import string

def generate_file(filename, size_in_bytes):
    # Generate random content of the specified size
    with open(filename, 'w') as f:
        for _ in range(size_in_bytes):
            f.write(random.choice(string.ascii_letters))  # You can customize this to write specific content

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <filename> <size_in_bytes>")
        sys.exit(1)

    filename = sys.argv[1]
    try:
        size_in_bytes = int(sys.argv[2])
        # size_in_bytes = 1 <<10
    except ValueError:
        print("Size must be an integer.")
        sys.exit(1)

    generate_file(filename, size_in_bytes)
    print(f"File '{filename}' with size {size_in_bytes} bytes created.")
