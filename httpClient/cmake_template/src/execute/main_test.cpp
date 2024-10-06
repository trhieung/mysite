#include <iostream>

int main(){
    int a = 0;
    int sz = 4;
    std::string str = " hihi\n";
    while(++a < sz){
        std::cout << a << str;
    }
    return 0;
}