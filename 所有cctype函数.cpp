//
// Created by AndyCui on 2023/10/11.
//
#include <string>
#include <iostream>
#include <utility>
#include <algorithm>
#include <cctype>       //包含标准 C 库标头 <ctype.h> 并将关联名称添加到 std 命名空间

namespace jt::cctype
{
    using namespace std;
    class MyClass
    {
    public:
        explicit MyClass(int size) : size(size) {}

        friend ostream &operator<<(ostream &os, const MyClass &aClass) {
            os << "nums: " << aClass.nums << " price: " << aClass.price << " items: " << aClass.items << " odvs: "
               << aClass.odvs << " size: " << aClass.size;
            return os;
        }

        MyClass(const MyClass& other) {
            size = other.size;

            nums = new int[10];
            for(int i = 0; i < 10; i++) {
                this->nums[i] = other.nums[i];
            }

            this->odvs = new float [size];
            for(int i = 0; i < size; i++) {
                this->odvs[i] = other.odvs[i];
            }

            for (int i = 0; i < sizeof(price)/sizeof(float); i++) {
                this->price[i] = other.price[i];
            }

            for (int i = 0; i < 10; ++i) {
                this->items[i] = other.items[i];
            }

        }

        /**
         * 移动构造函数使用的场景：当用临时对象初始化当前类的对象时，如果类同时定义了拷贝构造函数，移动构造函数，那么编译器很聪明，它会优先调用
         * 移动构造函数来完成这个操作。只有当类中没有合适的移动构造函数时，编译器才会退而求其次调用拷贝构造函数。
         *
         * 为什么要优先调用移动构造函数呢，因为反正是用临时对象去初始化当前类的对象，执行完成后临时对象也就没用了，所以不用白不用，在它死之前再利用它一次。
         * @param other 右值引用，可以直接传临时对象进来，不能传递左值对象进来，如果非要传左值，那么用std::move，而且传递完成后，原来的左值对象就不能用了，
         * 因为它的资源已经被移动给当前要初始化的类对象了。
         */
        MyClass(MyClass&& other) noexcept : size(other.size)  {
            //
            this->size = other.size;

            /*if(other.nums != nullptr) {
                this->nums = new int [10];
                for(int i = 0; i < 10; i++) {
                    this->nums[i] = other.nums[i];
                }
                other.nums = nullptr;
            }

            if(other.odvs != nullptr) {
                this->odvs = new float [size];
                for(int i = 0; i < size; i++) {
                    this->odvs[i] = other.odvs[i];
                }
                other.odvs = nullptr;
            }*/
            this->nums = nullptr;
            this->odvs = nullptr;
            //交换2个对象指向的指针，这意味着资源（如动态分配的内存）将从一个对象转移到另一个对象
            std::swap(nums, other.nums);
            std::swap(odvs, other.odvs);

            for (int i = 0; i < sizeof(price)/sizeof(float); i++) {
                this->price[i] = other.price[i];
            }
            for (int i = 0; i < 10; ++i) {
                this->items[i] = std::move(other.items[i]);
            }
        }

        virtual ~MyClass() {
            delete[] nums;
            delete[] odvs;
            cout << "被析构了" << endl;
        }

    private:
        int* nums{};
        float price[10]{};
        string items[10];
        float* odvs{};
        int size{10};
    };
}

int main(int argc, char** argv)
{
    using namespace jt::cctype;

    MyClass obj1{15};
    MyClass obj2(obj1);

    MyClass obj3(MyClass{22});      //传入临时对象，所以编译器调用的是移动构造函数

    int v1 = 153;
    auto r = std::isalnum(v1);
    std::islower(v1);
    std::isupper(v1);

    //std::cout是线程安全的吗？参考c++标准库中的类的线程安全性规格：https://learn.microsoft.com/zh-cn/cpp/standard-library/thread-safety-in-the-cpp-standard-library?view=msvc-170
//    标准 iostream 对象 cin、cout、cerr、clog、wcin、wcout、wcerr 和 wclog 遵循与其他类相同的规则，但存在此例外：可以安全地从多个线程写入一个对象。
//    例如，可以将线程 1 和线程 2 同时写入 cout。 但是，此操作可能会导致两个线程的输出相混合。
}
