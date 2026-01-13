//
// Created by AndyCui on 2023/10/14.
//
#include <iostream>
#include <memory>

class BufferedIO
{
public:
    enum class IOError {
        None,
        Access,
        General
    };

    class BufferedInput{
    public:
        int read() {
            return 0;
        }

        int good()
        {
            return this->_input_error == IOError::None;
        }
    private:
        IOError _input_error;
    };

    class BufferedOutput
    {

    };

private:
};


#include <iostream>
#include <memory>

void modifyObject(std::shared_ptr<int> ptr) {
    // 修改拷贝的shared_ptr所指向的对象不会影响原始的shared_ptr
    *ptr = 10;
}

void modifySharedPtr(std::shared_ptr<int>& ptr) {
    // 修改原始的shared_ptr所指向的对象
    *ptr = 20;
}

/**
 * BufferedIO::BufferedInput 和 BufferedIO::BufferedOutput 在 BufferedIO 内声明。
 * 这些类名称在类 BufferedIO 的范围外不可见。
 *
 * 但是BufferedIO类里面没有任何成员变量的类型是这些嵌套类。
 */
int main(int argc, char* argv[])
{
    BufferedIO::BufferedInput input{};
    BufferedIO::BufferedOutput output{};

    std::shared_ptr<int> objPtr = std::make_shared<int>(5);
    std::cout << "Original object value: " << (*objPtr) << std::endl;

    modifyObject(objPtr);
    std::cout << "After modifyObject: " << (*objPtr) << std::endl; // 输出5，modifyObject没有改变原始的shared_ptr所指向的对象

    modifySharedPtr(objPtr);
    std::cout << "After modifySharedPtr: " << (*objPtr) << std::endl; // 输出20，modifySharedPtr改变了原始的shared_ptr所指向的对象

}
