from pathlib import Path

class projectPath:
    @staticmethod
    def get_project_root():
        return (Path(__file__).resolve().parent / "..").resolve()
    
    @staticmethod
    def get_src_dir():
        return projectPath.get_project_root() / "src"
    
    @staticmethod
    def get_src_ctest_dir():
        return projectPath.get_src_dir() / "c_test"
    
    @staticmethod
    def get_payloads_dir():
        return projectPath.get_project_root() / "payloads"

    @staticmethod
    def get_payloads_bin_dir():
        return projectPath.get_payloads_dir() / "bin"
    
    @staticmethod
    def get_payloads_asm_dir():
        return projectPath.get_payloads_dir() / "asm"
    
    @staticmethod
    def get_bin_baseCTest_dir() -> str:
        return (projectPath.get_src_ctest_dir() / "../../payloads/bin").resolve()

    
