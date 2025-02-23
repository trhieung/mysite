#include <iostream>
#include <Windows.h>

extern "C" __declspec(dllexport) void testOpcodes(unsigned char* p, size_t size) {
	// Get space for shellcode

	void *sc = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// Copy shellcode and execute it
	try
	{
		memcpy(sc, p, size);
		(*(int(*)()) sc)();
	}
	// catch block to catch the thrown exception
    catch (const std::exception& e) {
        // print the exception
        std::cout << "Exception " << e.what() << std::endl;
    }
}

extern "C" __declspec(dllexport) int myadd(int a, int b) {
    return a + b;
}

int mysub(int a, int b) {
    return a - b;
}

int mymul(int a, int b) {
    return a * b;
}

int main() {
    printf("Hello, World!");
    printf("5 + 3 = %d\n", myadd(5, 3));
    printf("5 - 3 = %d\n", mysub(5, 3));
    return 0;
}
