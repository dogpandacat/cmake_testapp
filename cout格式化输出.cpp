//
// Created by AndyCui on 2023/10/13.
//

#include <bitset>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
using namespace std;

std::string PrintHex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    ss << showbase<< std::hex << std::setfill('0');
    for (const auto& b : bytes) {
        ss << std::setw(2) << static_cast<unsigned int>(b);
    }
    return ss.str();
}
typedef struct {
    uint32_t bits0_19 : 20; // 第0-19位含义
    uint32_t reserved1 : 3;  // 保留位，第23-25位
    uint32_t bits26_29 : 5; // 第26-29位含义
    uint32_t bit30 : 1;    // 第30位含义
    uint32_t bit31 : 1;    // 第31位保留
} Register;

int main() {
    int number = 10;
    std::bitset<32> binary(number);

    //showbase是在输出的前面加上"0b"表示这是二进制，setbase(2)表示2进制
    std::cout << std::showbase << std::setbase(2) << binary << std::endl;
    std::cout << std::hex << std::setw(6) << std::setfill('0') << number << endl;



    Register registerVar = {0x12345678, 0, 0x8F6, 0x1, 0}; // 示例变量

    // 设置输出格式为16进制，并使用setw和setfill设置字段宽度和对齐方式
    std::cout << std::hex << std::setw(8) << std::setfill('0') << registerVar.bits0_19 << " ";
    std::cout << std::hex << std::setw(3) << std::setfill('0') << registerVar.reserved1 << " ";
    std::cout << std::hex << std::setw(4) << std::setfill('0') << registerVar.bits26_29 << " ";
    std::cout << std::hex << std::setw(1) << std::setfill('0') << registerVar.bit30 << " ";
    std::cout << std::hex << std::setw(1) << std::setfill('0') << registerVar.bit31 << "\n";


    string s1 = PrintHex({1,2,3,4,5});
    cout << "s1 = " << s1 << endl;

    string s2 = std::move(PrintHex({10,20,30,40,50,60}));
    cout << "s2 = " << s2 << endl;


    return 0;
}