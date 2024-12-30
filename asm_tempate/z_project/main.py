
def hexToDec(val: str) -> int:
    # Convert hexadecimal string to decimal
    return int(val, 16)

def readFile(filename: str, idx: int, sz: int) -> bytes:
    # Open the file in read mode
    with open(filename, 'rb') as file:
        # Move the file pointer to the index position
        file.seek(idx)
        # Read the specified number of characters
        return file.read(sz)
    
exclamation_mark = b'\x21'
hex_val = "00000099"
print(hexToDec(hex_val))
print(readFile("./z_project/main.o", hexToDec(hex_val), 1))