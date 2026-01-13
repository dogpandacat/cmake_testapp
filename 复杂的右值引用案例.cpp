//
// Created by AndyCui on 2023/10/11.
//

#include <iostream>
#include <vector>


//微软提供的移动构造函数和右值引用教程：
//https://learn.microsoft.com/zh-cn/cpp/cpp/move-constructors-and-move-assignment-operators-cpp?view=msvc-170

using namespace std;
class Nested {
public:
    int value;

    explicit Nested(int val) : value(val) {}
    ~Nested() = default; // Nested类的析构函数
};

class Complex {
public:
    Nested* nestedPtr; // 指针类型的嵌套类变量
    int* arrPtr[10]{}; // 指针类型的数组变量，数组里面每个都是指针，声明数组的时候要加上{}
    int size;
    std::vector<int*> vec; // 里面每个元素都是int*

    Complex(int val, int arr_size) : nestedPtr(new Nested(val)), size(arr_size) {
        for (int i = 0; i < arr_size; i++) {
            arrPtr[i] = new int(i);
        }
        vec.resize(arr_size);
        for (int i = 0; i < arr_size; i++) {
            vec[i] = new int(i);
        }
    }

    // 移动构造函数
    Complex(Complex&& other) noexcept {
        nestedPtr = other.nestedPtr;
        size = other.size;
        vec = std::move(other.vec);
        other.vec.clear();

        nestedPtr->value +=10;
        //将other对象的成员恢复默认值。 这可以防止析构函数多次释放资源（如内存），double free问题是很严重的
        other.nestedPtr = nullptr;

        for (int i = 0; i < size; i++) {
            arrPtr[i] = other.arrPtr[i];        //指针的移动，只需要直接赋值就行了，比如ptr1指向某个地址，现在直接让ptr2=ptr1，ptr2也就指向同样的地址了

            //需要将other对象的arrPtr数组中的每个指针都恢复nullptr，否则会导致析构函数中出现double free
            //析构函数中会判断arrPtr数组中的每个元素是否=nullptr，当不等于nullptr的时候会进行delete释放
            other.arrPtr[i] = nullptr;
        }
    }

    // 拷贝构造函数，注意看堆nestedPtr和移动构造函数的处理是不一样的，这里既然是拷贝，就必须重新new一块内存空间
    // 另外对vec的初始化也是通过拷贝构造，而不是像上面vec(std::mov(other.vec))
    Complex(const Complex& other) : nestedPtr(new Nested(*other.nestedPtr)), size(other.size), vec(other.vec) {
        for (int i = 0; i < size; i++) {
            arrPtr[i] = new int(*other.arrPtr[i]);
        }
    }

    ~Complex() { // Complex类的析构函数
        if(nestedPtr != nullptr) {
            int id = nestedPtr->value;
            std::cout << "Complex" << id << "开始析构..." << std::endl;

            delete nestedPtr;
            nestedPtr = nullptr;
            std::cout << "Complex" << id << "析构1" << std::endl;

            //arrPtr本身是数组，分配在栈上不需要delete，但是它里面放的元素都是分配在堆上的，需要delete
            for (int i = 0; i < size; i++) {
                if (arrPtr[i] != nullptr) {
                    delete arrPtr[i];
                }
            }
            std::cout << "Complex" << id << "析构2" << std::endl;

//            for (auto & i : vec) {
//                delete i;
//            }
            for (int* pInt : vec) {
                delete pInt;
            }
            vec.clear();
            std::cout << "Complex" << id << "析构3" << std::endl;
        }
    }

    void print() {
        std::cout << "Nested value: " << nestedPtr->value << std::endl;
        std::cout << "Array: ";
        for (int i = 0; i < size; i++) {
            std::cout << *arrPtr[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Vector: ";
        for (int i = 0; i < vec.size(); i++) {
            std::cout << *vec[i] << " ";
        }
        std::cout << std::endl;
    }
};

class MyClass final {   //final控制这个类不允许被继承
public:
    // 构造函数
    MyClass() {
        data1 = new int[100];
        data2 = std::make_unique<int[]>(100);

        data.push_back(new int(0));
        for (int i = 0; i < 10; i++) {
            arrPtr[i] = new int(i);
        }
    }

    // 移动构造函数
    MyClass(MyClass&& other) noexcept {
        data = std::move(other.data);
        other.data.clear();

        for (int i = 0; i < 10; i++) {
            arrPtr[i] = other.arrPtr[i];
            other.arrPtr[i] = nullptr;
        }

        data1 = other.data1;
        other.data1 = nullptr;
        data2 = std::move(other.data2);
    }

    void print() {
        for(auto &i : arrPtr) {
            if(i == nullptr) {
                std::cout << "nullptr" << std::endl;
            } else {
                std::cout << *i << std::endl;
            }
        }
    }
    // 析构函数
    ~MyClass() {
        if(data1 != nullptr) {
            delete[] data1;
        }

        for (int* ptr : data) {
            delete ptr;
        }
        data.clear();

        for (auto & i : arrPtr) {
            if(i != nullptr) {  //为什么加这句判断，因为在移动构造函数中other.arrPtr[i] = nullptr;
                delete i;
                i = nullptr;
            }
        }
        std::cout << "MyClass被析构" << std::endl;
    }

private:
    std::vector<int*> data;  // vector存储指向整数的指针
    int* arrPtr[10]{};         // 数组存储指向整数的指针


    int* data1;
    std::unique_ptr<int[]> data2;   //析构函数中不再需要手动释放内存
};

int main() {

    {
        MyClass c1;
        MyClass c2(std::move(c1));
        c2.print();
        c1.print();
        int* p1 = new int(5);
        delete p1;

    }

    {
        Complex obj1(10, 5);
        obj1.print();
    }

    Complex obj2(20, 10);
    obj2.print();

    // Move construction
    Complex obj3(std::move(obj2));
    obj3.print();

    {

        std::vector<int> vec{ 0,1,2,3,4 };
        for (auto& num : vec) {
            num = num + 1;
        }

        for (auto num : vec) {
            cout << num << " ";
        }

        for (auto it = begin(vec); it != end(vec); it++)
        {
            // Access element using dereference operator
            cout << *it << " ";
            if(std::is_pointer<decltype(it)>::value) {
                cout << "auto it 是指针" << endl;
            }
        }
    }
    return 0;
}