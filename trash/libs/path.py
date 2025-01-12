from pathlib import Path
import libs._global as _global

class proPath:
    @staticmethod
    def getRoot():
        return (Path(__file__).resolve().parent / "..").resolve()
    def getProjects():
        return proPath.getRoot() / _global.dir_projects
    
    @staticmethod
    def getSubProNames() -> list[str]:
        ret:list = []
        subProPath =  proPath.getProjects()

        # Ensure the path exists and is a directory
        if subProPath.exists() and subProPath.is_dir():
            # Iterate through subdirectories and collect their names
            ret = [folder.name for folder in subProPath.iterdir() if folder.is_dir()]
        
        return ret
    
    
