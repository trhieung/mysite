#include <stdio.h>

// Declare the assembly functions
extern int add(int a, int b);
extern int add_four(int a, int b, int c, int d);

int main()
{
    int result1 = add(3, 5);            // Calls the add function
    int result2 = add_four(1, 2, 3, 4); // Calls the add_four function

    printf("Result of add: %d\n", result1);
    printf("Result of add_four: %d\n", result2);

    return 0;
}
