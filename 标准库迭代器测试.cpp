//
// Created by AndyCui on 2023/10/19.
//

#include <iostream>
#include <unordered_set>
#include <set>
#include <iterator>
#include <vector>
#include <type_traits>

using namespace std;




//演示<iterator>头文件中的advance函数，它实现的功能就是把迭代器指针往前挪N个位置，类似it++
void advance_demo()
{

}

void back_insert_demo()
{
    vector<int> vec{0, 1, 2, 3};
    for(int i = 4; i < 8; ++i)
    {
        vec.push_back(i);
    }
    cout << "vec初始值 = ";
    for (auto it = vec.cbegin(); it != vec.cend(); it++) {
        cout << *it << " ";
    }
    cout << endl;

    //这个类感觉没有特别用处，要往vector末尾插入值，直接用vec提供的函数就行了
    back_insert_iterator<vector<int>> binserter = std::back_inserter(vec);
    *binserter = 100;
    binserter++;
    *binserter = 200;

    //下面直接写=语法，其实是因为back_insert_iterator类重载了=操作符，它的operator=函数其实就是调用了container.push_back(xxx)
    //所以这里的binserter = 300;其实就是调用了vec.push_back(300)；语法是精炼了，但是可读性，一看就懂的效果就不明显了，对初学者确实不够友好
    //可能是标准委员会想让c++的代码更精炼，用较少的代码实现同样的功能吧。但是反过来说，c++的代码行数要比python多多了（实现同样的功能）
    binserter = 300;
    binserter = 400;

    cout << "vec后来的值 = ";
    for (auto it = vec.cbegin(); it != vec.cend(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

namespace abc
{
    /**
     * 下面这个模板函数是从src迭代器，拷贝n个元素到dest迭代器
     * 假设InIt迭代器指向vec1的第5个位置，OutIt迭代器指向vec2的第3个位置，现在调用copy_n(in, 5, out)
     * 那就会把vec1的第5~9个元素赋值给vec2的第3~7个元素，等于说是修改了vec2的第3~7个元素的值
     */
    template <class InIt, class OutIt>
    inline std::pair<InIt, OutIt> copy_n(
            InIt b, typename std::iterator_traits<InIt>::difference_type n, OutIt d) {
        for (; n != 0; --n, ++b, ++d) {
            *d = *b;
        }
        return std::make_pair(b, d);
    }
}


/**
 * C++标准库中迭代器是指针的泛化，用法和指针很类似，迭代器和指针一样支持++操作，迭代器也是指向容器中的某个位置，要获取这个位置的元素值，也要用*it解引用才能获得，
 * 这一点和指针一模一样，所以说它是指针的泛化，这个泛化可以说它是指针的一种表现形式吧
 *
 * 有了迭代器，就可以用同一种编程方式来处理不同的容器，代码写法都是一样的，<iterator>头文件中有很多函数和结构体：
 *
 * advance
 * begin
 * end
 * back_inserter    给定一个容器对象，传给back_inserter函数，它返回一个std::back_insert_iterator类的对象，它是一个可以在指定容器的后面插入元素的迭代器
 * cbegin   返回的是带const的迭代器，这就意味着不能通过这个const迭代器修改容器内元素的值，只能读取，要支持读写的话，调用begin函数
 * cend
 * data
 * next     让迭代器往前走指定的元素，就好像你在数东西，现在跳过N个，继续开始数一样的
 * prev     和next反过来
 *
 */
int main(int argc, char* argv[])
{
    cout << "测试迭代器..." << endl;
    back_insert_demo();

    vector<int> vec1{1,2,3,4,5,6,7,8,9,10};
    vector<int> vec2{10,20,30,40,50,60,70,80,90,100};

    //调用copy_n模板函数的时候，用下面2种写法都可以，用decltype会更简洁一些，但是可读性不佳，第2种写法一目了然，虽然啰嗦了点
    auto r = abc::copy_n<decltype(vec1.begin()), decltype(vec2.begin())>(vec1.begin(), 3, vec2.begin());
//    auto r = abc::copy_n<vector<int>::iterator, vector<int>::iterator>(vec1.begin(), 3, vec2.begin());
    cout << "typeid(r).name() = " << typeid(r).name() << endl;
    cout << *r.first << endl;
    cout << *r.second << endl;

    for (auto it = vec1.cbegin(); it != vec1.cend(); it++) {
        cout << *it << " ";
    }
    cout << endl;

    for (auto it = vec2.cbegin(); it != vec2.cend(); it++) {
        cout << *it << " ";
    }
    cout << endl;

    //从上面的代码执行结果来看，copy_n的功能确实如doc描述，起到用第一个容器中的值，修改第2个容器中的值的目的，而且是通过迭代器的方式来进行的
    //其实如果不用迭代器，直接遍历2个容器，也是可以做到的，但是迭代器是STL中操作容器的官方做法，所以必须使用。

    int val = 0;
    std::add_lvalue_reference_t<int> p = (int&)val;
    p = p;  // to quiet "unused" warning
    cout << "add_lvalue_reference_t<int> == " << typeid(p).name() << endl;
    cout << "add_lvalue_reference_t<int> == " << typeid(decltype(p)).name() << endl;
}