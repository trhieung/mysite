# my_class.py
from src._paths import projectPath

class _payloads:

    @staticmethod
    def _asm_from_bytes(_raw: bytes, _file_name: str, _global_name: str) -> None:
        # define path
        _asm_path = projectPath.get_payloads_asm_dir() / f"{_file_name}.s"
        _bin_path = projectPath.get_payloads_bin_dir() / f"{_file_name}.bin"
        _bin_baseADM_path = projectPath.get_bin_baseCTest_dir() / f"{_file_name}.bin"
        
        # Write raw bytes to a binary file
        with open(_bin_path, "wb") as f:
            f.write(_raw)

        # Create an assembly file that includes the binary file
        asm_content =   f"""
    .global {_global_name}
{_global_name}:
    .incbin "{_bin_baseADM_path}"
    """
        with open(_asm_path, "w") as f:
            f.write(asm_content)
