//
// Created by AndyCui on 2023/10/12.
//


#include <iostream>

#include <boost/timer/timer.hpp>
using namespace std;

class Base {
public:

    Base() {
        cout << "Base被构造" << endl;
    };

    virtual ~Base() {
        // 基类析构函数中的清理逻辑
        cout << "~Base invoked!" << endl;
    }
};

class Derived final : public Base {
public:

    Derived() {
        cout << "Derived被构造" << endl;
    };

    ~Derived() {
        // 派生类析构函数中的清理逻辑
        cout << "~Derived invoked!" << endl;
    }
};

int main(int argc, char* argv[])
{
    using Detected_CPU_TIME = boost::timer::auto_cpu_timer;
    Detected_CPU_TIME timer;
    //如果Base的析构函数不加virtual关键字，那么就不会调用子类的析构函数
    //所以一旦涉及到父子继承关系，最好是把基类的析构函数加上virtual关键字
    Base* b = new Derived;
    delete b;

    cout << "*******************************************" << endl;

    //子类指针指向子类对象没有问题，会自动调用子类析构函数，然后自动调用父类析构函数
    Derived* c = new Derived;
    delete c;

}