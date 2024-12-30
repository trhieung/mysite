#include <stdio.h>

extern void linux_64__bind_tcp(); // Declare the external assembly function

int main() {
    linux_64__bind_tcp(); // Call the external function
    return 0;
}
