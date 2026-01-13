//
// Created by AndyCui on 2023/10/20.
//

#include <iostream>
#include "point.h"
#include <ctime>
#include <iomanip>
#include <type_traits>

using namespace std;


#include <type_traits>
#include <iostream>

int test_add_pointer()
{
    //p本来数据类型是int，给它套上std::add_pointer_t<int>后，它的数据类型就变成了int*，这完全是用于模板元编程的
    //如果是正常编程，傻逼才会这样写，直接写int*就完事了
    int value = 100;
//    std::add_pointer_t<int> p = static_cast<int*>(nullptr);
    std::add_pointer_t<int> p = &value;

    p = p;  // to quiet "unused" warning
    std::cout << "add_pointer_t<int> == "
              << typeid(p).name() << std::endl;

    return (0);
}


void test_std_is_nothrow_copy_assignable()
{
    struct MyClass {
        MyClass(const MyClass&) = delete; // 删除拷贝构造函数以禁止拷贝
        MyClass& operator=(const MyClass&) = delete; // 删除拷贝赋值运算符以禁止拷贝
    };

    static_assert(!std::is_nothrow_copy_assignable<MyClass>::value, "MyClass must not be copyable");
    std::cout << "OK" << std::endl;
}

/**
 * 演示<iomanip>头文件中的操控器函数，它用于操作cout，这里先演示一下get_time，其实这个函数没啥鸟用，自己写个解析比什么都好用，
 * 而且有很多专门的读写时间日期的库，比这个号太多了
 * @return
 */
int test_put_time()
{
    std::cout << "Enter a time, for example 15:24 for 3:24pm: ";
    struct std::tm when;
    std::cin >> std::get_time(&when, "%R");
    if (!std::cin.fail())
    {
        std::cout << "Entered: " << when.tm_hour << " hours, " << when.tm_min << " minutes\n";
    }

    return (int)std::cin.fail();
}

int main(int argc, char* argv[])
{
    cout << "start test geom3d" << endl;
    test_add_pointer();
    test_std_is_nothrow_copy_assignable();

    test_put_time();
}