#include <iostream>
#include "header1.h"
#include "header2.h"

int notionMain1()
{
    std::cout << "Hi from header main 1\n";
    std::cout << "-----function ref--------\n";

    Header1 h1;
    h1.notionHeader(); // This should work if notionHeader is public

    Header2 h2;        // Ensure Header2 is declared in header2.h
    h2.notionHeader(); // This should also work

    return 0;
}

int main()
{
    return notionMain1();
}
