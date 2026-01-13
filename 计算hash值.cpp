//
// Created by AndyCui on 2023/10/17.
//

#include <string>
#include <iostream>
#include <boost/container_hash/hash.hpp>
#include <thread>
#include <chrono>
#include <memory>
#include <future>

using namespace std;

class MyClass {
public:
    int a{15};
    std::string b{"greate!"};
};

namespace std{
    template<>
    struct hash<MyClass> {
        // Define hash function for MyClass objects
        std::size_t operator()(const MyClass& obj) const {
            std::size_t h1 = std::hash<int>()(obj.a);
            std::size_t h2 = std::hash<std::string>()(obj.b);
            return h1 ^ (h2 << 1); // or use boost::hash_combine (see Note)
        }
    };
}

void sleep_second(int count)
{
    std::this_thread::sleep_for(std::chrono::seconds(count));
}

//毫秒级延时
void sleep_milli_seconds(int count) {
    std::this_thread::sleep_for(std::chrono::milliseconds(count));
}

//微秒级延时
void sleep_micro_seconds(int count) {

    this_thread::sleep_for(chrono::microseconds(count));
}

int main(int argc, char* argv[])
{
    MyClass obj1;
    cout << std::hash<string>{}("abcdefg") << endl;
    std::cout << "Hash of MyClass: " << std::hash<MyClass>{}(obj1) << std::endl;

    MyClass obj2;
    cout << std::hash<MyClass>{}(obj2) << endl;

    boost::hash<std::string> string_hash;
    std::size_t h = string_hash("Hash me");
    cout << "boost.hash计算结果：" << h << endl;

    sleep_second(3);
    cout << "exit after 3 seconds" << endl;

    sleep_milli_seconds(1000);
    cout << "delay 1000 milli seconds" << endl;

    //微秒级别延时，精度还是很高的
    sleep_micro_seconds(1);
    cout << "delay 1 micro seconds" << endl;
    cout << "now exit" << endl;

    std::unique_ptr<void*[]> p1 = std::make_unique<void*[]>(100);
    auto objects = p1.get();    //原始指针是void**二级指针，因为void*[]是指针数组
}