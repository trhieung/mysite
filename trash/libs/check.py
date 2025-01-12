from .path import proPath

class proCheck:
    @staticmethod
    def validProName(proName: str) -> bool:
        return (proName in proPath.getSubProNames())
