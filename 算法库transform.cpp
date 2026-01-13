//
// Created by AndyCui on 2023/10/10.
//

#include <list>
#include <algorithm>
#include <initializer_list>


#include <iostream>

using namespace std;

template<class Type>
class MultValue
{
private:
    Type Factor;

public:
    explicit MultValue(const Type& factor) : Factor(factor) {
        cout << "调用Type&构造函" << endl;
    }
    explicit MultValue(Type&& value) : Factor(value) {
        cout << "调用Type&&构造函数" << endl;
    }

    Type operator()(const Type& elem) const
    {
        return elem * Factor;
    }
};

int main(int argc, char** argv) {
    MultValue<int> a{10};
    int result = a(3000);
    cout << " 3000 * 10 = " << result << endl;

    //如果把explicit MultValue(const Type& factor)注释掉，那么MultValue的构造函数只支持右值，下面就会报错
    int factor = 100;
    MultValue<int> b{factor};

    //用std::move就不会报错了，左值转换为右值，调用的是MultValue(Type&& value)
    MultValue<int> c{std::move(factor)};

    //因为构造函数有explicit，所以不允许隐式转换，下面的d无法编译通过
//    MultValue<int> d = 200;
}