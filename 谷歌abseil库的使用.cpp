//
// Created by AndyCui on 2023/10/18.
//

#include <absl/hash/hash.h>
#include <absl/strings/str_format.h>
#include <string>
#include <iostream>

using namespace std;


#include <iostream>
#include <type_traits>
#include <list>
#include <memory>

void m1()
{

}

int m2(int a, int b)
{
    return a + b;
}

struct MyType {

//    ~MyType() = delete;
    MyType(const MyType&) = delete; // 删除拷贝构造函数
    MyType& operator=(const MyType&) = delete; // 删除拷贝赋值运算符
    MyType(MyType&&) = default; // 默认移动构造函数
    MyType& operator=(MyType&&) = default; // 默认移动赋值运算符
};



int main(int argc, char* argv[])
{
    //1. 测试它的字符串format功能
    std::string s = absl::StrFormat("Welcome to %s, Number %d!", "The Village", 6);
    cout << "result = " << s << endl;


    //2. 判断一个结构体、一个class是否支持移动操作，通过<type_traits>中的模板函数进行判断

    std::cout << std::boolalpha; // 输出布尔值时为 true 或 false，而不是 1 或 0
    std::cout << "Is MyType move constructible? " << std::is_move_constructible<MyType>::value << '\n';
    std::cout << "Is MyType move assignable? " << std::is_move_assignable<MyType>::value << '\n';

//    cout << typeid(std::is_copy_constructible<MyType>::value_type).name() << endl;
    cout << "是否支持拷贝构造？ " << std::is_copy_constructible<MyType>::value << endl;
    cout << "是否支持拷贝构造？ " << std::is_copy_constructible_v<MyType> << endl;

    cout << "是否支持拷贝赋值运算符？" << std::is_copy_assignable<MyType>::value << endl;
    cout << "是否支持拷贝赋值运算符？" << std::is_copy_assignable_v<MyType> << endl;

    //3. 测试string的move

    const char* str = "Hello, world!";
    std::printf("str指针的地址是：%p\n", &str);
    std::printf("str指向的字符串中第一个字符的地址是：%p\n", &str[0]);
    std::cout << "The memory address of the first character is: " << static_cast<void*>(const_cast<char*>(str)) << std::endl;

    list<string> list1{};
    string value1{"aaaa"};
    cout << "value1被移动前：" << value1.c_str() << endl;
    std::printf("value1被移动前的内存地址：%p\n", &(value1.c_str()[0]));

    //把一个左值引用通过std::move强制转换为右值引用
    list1.emplace(list1.cbegin(), std::move(value1));
    cout << "list1里面的元素：" << list1.back() << endl;
    std::printf("list1里面的元素的内存地址：%p\n", &(list1.back().c_str()[0]));

    //左值引用通过std::move强制转换为右值引用后，按照编程规范，后续的代码就不应该继续操作这个左值引用，因为它持有的资源已经被移动给另外一个对象了
    //所以如果继续操作这个左值引用，必然就会造成程序逻辑不对，因为这个左值引用持有的资源，持有的数据都已经是未知状态了
    //IDE也会给出智能提示，总之，不要再用这个左值引用了
    cout << "value1被移动后：" << value1.c_str() << endl;

    cout << std::addressof(m1) << endl;

    //下面是直接定义了一个匿名的结构体，同时创建了一个变量，也就是只能创建一次它的变量
    struct alignas(4) {
        int event_mask;
        bool enabled{false};
    } read_mostly_;

    read_mostly_.event_mask = 5;
    read_mostly_.enabled = true;

    if(read_mostly_.event_mask != 5)
    {
        throw std::invalid_argument("dataa");
    }

    cout << "read_mostly_.event_mask = " << read_mostly_.event_mask << endl;

    //因为上面是匿名结构体，所以下面编译器输出的就是编译器帮它生成的结构体名称了，会很长，辨识度很高
//   struct `int __cdecl main(int,char * __ptr64 * __ptr64 const)'::`2'::<unnamed-type-read_mostly_>
    cout << "read_mostly_变量的类型 = " << typeid(decltype(read_mostly_)).name() << endl;
}