//
// Created by AndyCui on 2023/10/13.
//


#include <iostream>
#include <string>
#include "modbus.h"

#include <random>
#include <limits>

using namespace std;
int test_modbus_tcp()
{
    //下面开始测试如何使用这个libmodbus动态库
    modbus_t *client = modbus_new_tcp("127.0.0.1", 502);
    if(client == nullptr) {
        cout << "libmodbus连接失败" << endl;
        return -1;
    }

    if(modbus_connect(client) != 0) {
        cout << "libmodbus连接失败" << endl;
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(client);
        return -1;
    }
//    modbus_set_slave(client, 1);  //只有RTU需要set slave id
    modbus_set_debug(client, 1);

    uint16_t values[2] = {0};           //一定要初始化为0
    memset(values, 0, sizeof(values));
    modbus_read_registers(client, 0, 2, values);
    //打印数据
    cout << "modbus寄存器中的原始数据是：" << endl;
    cout << "values[0] = " << values[0] << endl;
    cout << "values[1] = " << values[1] << endl;

    //现在继续测试往里面写
    uint16_t datas[2] = {0};
    memset(datas, 0, sizeof(datas));
    datas[0] = 3333;
    datas[1] = 4444;
    modbus_write_registers(client, 0, 2, datas);

    //写进去后继续读出来看看是不是对的
    modbus_read_registers(client, 0, 2, values);
    cout << "写入数据完成后再次读出，看看数据是否正确" << endl;
    cout << "values[0] = " << values[0] << endl;
    cout << "values[1] = " << values[1] << endl;

    modbus_close(client);
    modbus_free(client);

    return 0;
}

/**
 * 随机生成条形码数据，13位长度的一个大整数
 * @return
 */
uint64_t  get_13_length_int_value()
{
// 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(3344556677789, 9999999999999);
//    std::uniform_int_distribution<uint64_t> dis(5632147896325, std::numeric_limits<uint64_t>::max()); // 范围从指定的最小值到uint64_t的最大值
    // 生成一个大于等于5632147896325的随机数
    uint64_t randomValue = dis(gen);
    cout << "生成随机条形码数据：" << randomValue << endl;
    return randomValue;
}

int test_modbus_rtu()
{
    //测试modbus rtu
    modbus_t* client = modbus_new_rtu("COM2", 9600, 'N', 8, 1);
    if(client == nullptr) {
        cout << "modbus rtu建立失败" << endl;
        return -1;
    }
    auto r = modbus_connect(client);
    if(r == -1) {
        cout << "modbus rtu建立失败" << endl;
        modbus_free(client);
        return -1;
    }
    modbus_set_slave(client, 1);
    modbus_set_debug(client, 0);

    uint16_t values[2] = {0};           //一定要初始化为0
    memset(values, 0, sizeof(values));
    modbus_read_registers(client, 0, 2, values);
    //打印数据
    cout << "modbus寄存器中的原始数据是：" << endl;
    cout << "values[0] = " << values[0] << endl;
    cout << "values[1] = " << values[1] << endl;

    //现在继续测试往里面写
    uint16_t datas[2] = {0};
    memset(datas, 0, sizeof(datas));
    datas[0] = 33;
    datas[1] = 44;
    modbus_write_registers(client, 0, 2, datas);

    //写进去后继续读出来看看是不是对的
    modbus_read_registers(client, 0, 2, values);
    cout << "写入数据完成后再次读出，看看数据是否正确" << endl;
    cout << "values[0] = " << values[0] << endl;
    cout << "values[1] = " << values[1] << endl;


    cout << "开始测试32位整数数据读写......" << endl;

    //modbus的寄存器都是16bit，2个字节，如果遇到需要超过65536的数据，要么默认放大100倍，要么占用连续
    //的2个寄存器，这样就合并为4个字节，32bit
    uint16_t results[2] = {0};

    //从地址0x03开始读2个modbus寄存器的值，2个16bit合并为32bit
    //从0x03开始的2个寄存器合并为一个32bit整数，按big-endian格式存储，最高有效位存储在内存低地址处，最低有效位存储在内存高地址处
    modbus_read_registers(client, 0x03, 2, results);
    //所以results[0]存储的是32位整数的高位字节，所以要把results[0]左移16bit
    int32_t r2 = ((int32_t)results[0] << 16) | (int32_t )results[1];
    cout << "32位signed有符号整数（big-endian） = " << r2 << endl;

    cout << "*********************************************************************" << endl;
    int _64register_addr = 0x03;
    cout << "开始测试读取64位 unsigned 整数（big-endian）"<< endl;
    uint16_t code[4] = {0};
    modbus_read_registers(client, _64register_addr, 4, code);
    uint32_t codeHigh = ((uint32_t)code[0] << 16) | (uint32_t)code[1];
    uint32_t codeLow = ((uint32_t)code[2] << 16) | (uint32_t)code[3];
    uint64_t final_result = ((uint64_t)codeHigh << 32) | codeLow;
    cout << "原来modbus寄存器存放的64位无符号整数（big-endian） = " << final_result << endl;

    cout << "开始写入64位 unsigned 整数（big-endian）到modbus寄存器中"<< endl;
    //测试写入64位无符号整数（big-endian），很大的一个值，需要写入4个modbus寄存器
    uint64_t testValue = get_13_length_int_value();
    auto high16Bits1 = static_cast<uint16_t>(testValue >> 48);
    auto high16Bits0 = static_cast<uint16_t>((testValue >> 32) & 0xFFFF);
    auto low16Bits1 = static_cast<uint16_t>((testValue >> 16) & 0xFFFF);
    auto low16Bits0 = static_cast<uint16_t>(testValue & 0xFFFF);

    uint16_t temp[4] = {0};   //本质是就是把64位的整数拆开成4个16位的值写入
    temp[0] = high16Bits1;
    temp[1] = high16Bits0;
    temp[2] = low16Bits1;
    temp[3] = low16Bits0;
    modbus_write_registers(client, _64register_addr, 4, temp);
    cout << "将新的64位无符号（big-endian）值到modbus寄存器中..." << endl;

    cout << "再次读取64位 unsigned 整数（big-endian），检查是否写入成功..."<< endl;
    modbus_read_registers(client, _64register_addr, 4, code);
    codeHigh = ((uint32_t)code[0] << 16) | (uint32_t)code[1];
    codeLow = ((uint32_t)code[2] << 16) | (uint32_t)code[3];
    final_result = ((uint64_t)codeHigh << 32) | codeLow;
    cout << "重新读取写入后的64位无符号（big-endian）值 = " << final_result << endl;
    if(testValue == final_result) {
        cout << "64位无符号大整数（big-endian）写入测试成功 " << endl;
    } else {
        cerr << "64位无符号大整数（big-endian）写入测试失败！！！！！！！！！ " << endl;
    }

    cout << "*********************************************************************" << endl;
    cout << "开始进行32位有符号整数（little-endian）读数据测试...";
    //测试：little-endian，在modbus寄存器中，数据按little-endian存储，最高有效位存储在内存高地址处，低字节存储在内存低地址
    modbus_read_registers(client, 0x05, 2, results);
    /**
     * htons 函数位于 <netinet/in.h> 头文件中，并将短整型转换为网络字节顺序。它可以确保每个平台上都有一致的结果。
    此外，htonl 函数也位于此头文件中，用于将长整型转换为网络字节顺序。
     */
    uint16_t low = htons(results[0]);
    uint16_t high = htons(results[1]);

    int32_t r3 = ((int32_t)high << 16) | low;
    cout << "32位signed有符号整数（little-endian） = " << r3 << endl;

    cout << "测试读32位float数据，big-endian，这里面用到union这个技巧，非常经典" << endl;
    union {
        float val;
        uint16_t res[2];
    } d{};
    //因为modbus slave那边设定的是按big-endian存储32位float数据，用2个寄存器一共4个字节来表示这个float数据
    //big-endian是高位字节在内存低地址处，d.res[0]存放的是高地址modbus寄存器的值，d.res[1]存放的是低地址modbus寄存器的值
    //所以通过swap把它们更换一下位置顺序，最终可以看到是正确的
    modbus_read_registers(client, 0x07, 2, d.res);
    std::swap(d.res[0], d.res[1]);
    printf("32位float数据（Big-endian），结果 = %.3f\n", d.val);

    modbus_close(client);
    modbus_free(client);

    return 0;
}


/**
 * https://mp.weixin.qq.com/s/2J_aZzs23O9PYdzeFlbWZw
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char* argv[])
{
#if 1
    test_modbus_rtu();
#else
    test_modbus_tcp();
#endif

    cout << "*****************************************************************************" << endl;

    /**
     * windows和x86 cpu都是little-endian，linux两种模式都支持，但是要修改的话，要重新编译内核，已经安装好的linux，可以通过下面的程序判断
     *
     *
     * #include <endian.h>

            if (__BYTE_ORDER == __LITTLE_ENDIAN)
                std::cout << "Little-Endian" << std::endl;
            else if (__BYTE_ORDER == __BIG_ENDIAN)
                std::cout << "Big-Endian" << std::endl;
            else
                std::cout << "Unknown byte order" << std::endl;
     *
     *
     */
    short v = 0x1234;
    unsigned char* p = (unsigned char*)&v;

    //下面分别输出0x34和0x12，因为x86 cpu是little-endian，把一个数据的低位字节存储在内存中低地址处的，而p[0]是内存低地址，p[1]是内存高地址，所以p[1]存放的是0x12
    //windows系统也是little-endian
    std::cout << std::hex << static_cast<int>(p[0]) << std::endl;       //0x34
    std::cout << std::hex << static_cast<int>(p[1]) << std::endl;       //0x12

    return 0;
}