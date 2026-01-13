//
// Created by AndyCui on 2023/10/18.
// 体会一下自定义的内存分配器，为什么会有内存分配器？ 使用new和delete关键字，低层对应的是malloc和free函数，再或者用tcmalloc、jemalloc对编译器自带的malloc，free进行增强。
// 但是这些内存分配器也是有性能开销的，new关键字对应的malloc函数，也是有性能开销的，如果在一开始就预先分配好一块大内存，用的时候从这块大内存申请，就不
// 涉及使用new和delete关键字，一下子就把内存分配和释放的开销降低了很多。
// 下面演示一个最简单的案例

#include <memory>
#include <new>
#include <atomic>


using namespace std;

namespace abc {
    class MemoryAllocator1
    {
    private:
        bool in_use{false};
        std::atomic<bool> in_use2{false};
        char storage[1024]{0}; //直接就分配好的内存空间，初始状态是未使用，靠in_use标志位来控制

    public:
        explicit MemoryAllocator1(bool inUse) : in_use(inUse) {}

        void* allocate(std::size_t  size) {
            if(size <= sizeof(storage)) {
                in_use = true;          //set to true就表示内存已经被分配出去了
                return &storage;
            } else {
                //如果要分配的内存大于内部容量，就直接通过new操作符分配
                /*
                 * 在C++中，operator是一个用于重载运算符的特殊关键字。这意味着你可以使用这个关键字来重新定义运算符的行为。
                 * 以new运算符为例，在默认情况下，new会调用对象的构造函数来初始化新分配的对象。但是，如果你想改变这个行为，你可以使用operator new来重载它。
                 *
                 * 比如如果每次分配内存都要记录一条分配记录以便后续做分析，那就需要一个重载的new运算符版本，可通过重载operator new来实现。
                 * 下面的代码::operator new(size)就是在调用全局的、默认的new运算符
                 *
                 * :: 在这里表示的是全局作用域。它可以用来调用全局范围内的函数或对象。因此，::operator new 表示调用全局的 operator new 函数。
                 */
                return ::operator new(size);
            }
        }

        void deallocate(void*p, size_t  size)
        {
            if(p == &storage) {
                in_use = false;
            } else {
                /**
                 *  operator new 和 operator delete都是函数，而new、delete是关键字，形成表达式
                 */
                ::operator delete(p);
            }
        }
    };
}

/**
 * 编译当前文件 cl.exe /DWIN32 /D_WINDOWS /EHsc /Ob0 /Od /RTC1 -std:c++17 -MDd -Zi /utf-8 /Zs   xxx.cpp
 *
 * /DWIN32: 这是一个预处理器的选项，用于定义一个名为 'WIN32' 的宏。如果定义了这个宏，那么你可以在代码中使用 #ifdef WIN32 等来进行条件编译
 * /D_WINDOWS: 同样是一个预处理器的选项，定义一个名为 '_WINDOWS' 的宏。这个宏也可以用于条件编译。/EHsc: 这是编译器错误处理选项。'EHa' 表示异常处理，'sc' 表示堆栈溢出保护。/
 * /Ob0: 这是优化级别选项，'O0' 表示不进行优化。
 * /Od: 这个选项也与优化有关，表示禁用所有优化。
 * /RTC1: 这个选项启用实时编译错误检查，'1' 表示只检查第一遍，之后会跳过检查以提高编译速度。
 * -std:c++17: 这个选项指定编译器使用C++17标准。
 * -MDd: 这个是一个链接器选项，用于创建多线程调试信息，这种调试信息对于后续的调试过程非常有用，可以帮助程序员找出程序中的潜在问题
 *       这个选项在编译过程中会生成两种类型的调试信息，一种用于调试多线程程序中的同步问题，
 *       另一种用于调试多线程程序中的线程安全性问题。这些调试信息可以帮助程序员更好地理解多线程程序的行为，并找出其中的潜在问题。
 *       要注意的是，使用-MDd选项编译出的程序会比在发布模式下编译出的程序大很多，
 *       因为发布模式下省略了程序的调试信息。因此，在发布程序时，通常会使用-MD选项而不是-MDd选项。
 *
 * -Zi: 这个选项启用完全调试信息，包括局部变量和内部类型。
 * /utf-8: 这个选项指定源代码的编码格式为UTF-8。
 * /Zs: 这个选项让编译器在源文件中添加符号信息，以帮助调试，否则没法debug
 *
 * /Wall：显示所有告警
 */
int main(int argc, char* argv[])
{
}