#include <iostream>
#include "calc_lib.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    int result = add_values(1000, 2000);
    std::cout << "add_values of 1000 and 2000, result  = " << result;
    return 0;
}
