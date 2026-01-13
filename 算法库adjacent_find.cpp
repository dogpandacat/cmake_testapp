//
// Created by AndyCui on 2023/10/10.
//
#include <list>
#include <algorithm>
#include <initializer_list>


#include <iostream>

using namespace std;

bool twice(int elem1, int elem2) {
    return elem1 * 2 == elem2;
}

int main(int argc, char** argv) {
    list<int> L;
    L.push_back(50);
    L.push_back(40);
    L.push_back(10);
    L.push_back(20);
    L.push_back(20);

    list<int>::iterator iter;
    list<int>::iterator result1, result2;

    cout << "L = ( ";
    for(iter = L.begin(); iter != L.end(); iter++) {
        cout << *iter << " ";
    }
    cout << " ) " << endl;

//    搜索相等或满足指定条件的两个相邻元素

    result1 = adjacent_find(L.begin(), L.end());
    if(result1 == L.end()) {
        cout << "查找失败" << endl;
    } else {
        cout << "找到了：" << *result1 << endl;
    }

    result2 = adjacent_find(L.begin(), L.end(), twice);
    if(result2 == L.end()) {
        cout << "没找到2个元素，其中1个是另外一个的2倍" << endl;
    } else {
        cout << "找到了2个元素，第2个是第1个的2倍，这2个元素分别是：" << *result2 << " & " << *(++result2) << endl;
    }
}