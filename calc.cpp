#include <iostream>
#include "calc_lib.h"

int main() {
    std::cout << "测试一下calc_lib的功能是否正确" << std::endl;
    int result = add_values(100000, 2000);
    std::cout << "add_values of 1000 and 2000, result  = " << result;
    return 0;
}
