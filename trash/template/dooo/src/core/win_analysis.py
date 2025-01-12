class WinAnalysis:
    @staticmethod
    def getOffsets() -> list:
        return [
            {
                "name": "windows_x64_1903_18362.1",
                "offsets": {
                    'srvnet!SrvNetWskConnDispatch': 0x2D170,
                    'srvnet!imp_IoSizeofWorkItem': 0x32210,
                    'srvnet!imp_RtlCopyUnicodeString': 0x32288,
                    'nt!IoSizeofWorkItem': 0x12C410,
                    'nt!MiGetPteAddress': 0xBA968
                }
            },
        ]

    @staticmethod
    def showArchSupports():
        supports = WinAnalysis.getOffsets()
        print(f"{'Index':<10} {'Name':<40}")
        print("-" * 50)
        for idx, item in enumerate(supports):
            print(f"{idx:<10} {item['name']:<40}")

    @staticmethod
    def getOffset(idx: int):
        supports = WinAnalysis.getOffsets()
        if 0 <= idx < len(supports):
            return supports[idx].get("offsets")
        else:
            print("Invalid index. Please choose a valid index from the list.")
            return None


# Example usage:
import json
WinAnalysis.showArchSupports()
offsets = WinAnalysis.getOffset(0)
print("\nOffsets:", json.dumps(offsets, indent=2))
