//
// Created by AndyCui on 2023/10/17.
//
#include <jemalloc/jemalloc.h>
#include <iostream>
#include <optional>
#include <string>

using namespace std;

void do_something(size_t i){
    malloc(i*100);
    cout << "分配内存：" << i*100 << " 字节" << endl;
}

optional<string> find22(const string& src)
{
    if(src.empty()) {
        throw new std::runtime_error("src字符串是空的");
    }

    //第2个参数Off指的是从src的第几个位置开始查找
    if(src.find("andy", 2) == string::npos) {
        return std::nullopt;
    } else {
        return "found";
    }
}


int main(int argc,char** argv){
    for(size_t i=0;i<100;i++){
        do_something(i);
    }

    //测试一下optional类的使用方法，它能够确保返回值不会是null，因为返回值类型是std::nullopt

    auto result = find22("123andycui233");
    if (result.has_value()) {
        cout << "找到了：" << *result << endl;
    } else {
        cout << "没找到" << endl;
    }
    return 0;
}