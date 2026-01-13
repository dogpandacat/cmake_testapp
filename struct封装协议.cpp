//
// Created by AndyCui on 2023/10/12.
//

//开发的过程中，会遇到通信协议是按bit位定义的情况，比如协议一共6个byte字节，每个byte一共8bit位，但是传输的很多数据用1、2个bit就足够了，
// 这时协议会按bit定义，如何方便快捷的进行bit位的赋值和读取。一般都是在嵌入式开发中会这样做，因为带宽有限，所以精简协议，确保数据量很小，是非常重要的。


//byte0
//byte1
//byte2
//byte3-byte4
//byte5-byte6
//byte7-byte8
//...
//byte-n ~ byte-n+1

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

//利用结构体能设置占用bit长度的特性，来实现不同字段分别占用1bit、4bit、6bit的长度
template<uint8_t  N>        //这个写法也很有意思，里面没有typename，后续就要这样写：Result<8> result;
struct Result {
    //byte0
    uint8_t service : 4;        //占4个bit
    uint8_t type : 4;           //占4个bit

    // byte 1
    uint8_t sensor_0 : 1;  // 此变量占用1个bit
    uint8_t sensor_1 : 1;
    uint8_t sensor_2 : 1;
    uint8_t sensor_3 : 1;
    uint8_t sensor_4 : 1;
    uint8_t sensor_5 : 1;
    uint8_t obligate_0 : 2;  // 此变量占用2个bit

    // byte 2
    uint8_t obligate_1 : 4;  // 此变量占用4个bit
    uint8_t failure_count : 4;

    // byte3-...
    uint8_t failures[2 * N];  // 故障信息 占用2*N个byte
};

template<uint8_t N>
union UResult
{
    struct Result {
        //byte0
        uint8_t service : 4;        //占4个bit
        uint8_t type : 4;           //占4个bit

        // byte 1
        uint8_t sensor_0 : 1;  // 此变量占用1个bit
        uint8_t sensor_1 : 1;
        uint8_t sensor_2 : 1;
        uint8_t sensor_3 : 1;
        uint8_t sensor_4 : 1;
        uint8_t sensor_5 : 1;
        uint8_t obligate_0 : 2;  // 此变量占用2个bit

        // byte 2
        uint8_t obligate_1 : 4;  // 此变量占用4个bit
        uint8_t failure_count : 4;

        // byte3-...
        uint8_t failures[2 * N];  // 故障信息 占用2*N个byte
    } data;

    // 注意看这是一个union，这样就不需要手动把struct Result转换为vector<uint8_t>了，
    // 利用union的特性，直接访问u8data这个数组，就可以确保直接在字节层面访问到struct Result的数据了，非常的方便
    uint8_t u8data[3 + 2*N];
};

struct LidarFailReason
{
    uint8_t sensor_id : 4;
    uint8_t install_fail : 1;
    uint8_t point_cloud_too_few : 1;
    uint8_t localization_fail : 1;
    uint8_t no_model_data : 1;

    uint8_t obligate_0 : 8;
};

struct RadarFailReason
{
    //byte0
    uint8_t sensor_id : 4;
    uint8_t no_point_data : 1;
    uint8_t point_to_few : 1;
    uint8_t install_fail : 1;
    uint8_t obligate_0 : 1;

    //byte1
    uint8_t obligate_1 : 8;
};

struct Result2
{
    uint8_t a1 : 2;
    uint8_t a2 : 1;
};

std::string format_to_string(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    ss << std::showbase << std::hex << std::setfill('0');
    for (unsigned char byte : bytes) {
        ss << std::hex << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

/**
 * https://mp.weixin.qq.com/s/2J_aZzs23O9PYdzeFlbWZw
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    Result<8> result{};
    std::memset(&result, 0, sizeof(result));
    result.service = 10;
    result.type = 3;
    result.sensor_0 = 1;
    result.sensor_1 = 0;

    //往Result中填入错误信息，每个错误占2个字节，每种错误都用一个结构体表示，上面有2个示例错误结构体
    uint8_t count = 0;

    //怎么往Result的failures[2 * N]字段赋值呢？思路是把每2个字节强制转换为对应的错误信息结构体
    auto lidar = reinterpret_cast<LidarFailReason*>(&(result.failures[2*count]));
    count++;
    lidar->sensor_id = 1;
    lidar->point_cloud_too_few = 1;

    auto radar = reinterpret_cast<RadarFailReason*>(&(result.failures[2*count]));
    count++;
    radar->sensor_id = 2;
    radar->no_point_data = 1;

    result.failure_count = count;    //現在=2

    //怎么把这个Result大结构体中的数据以十六进制格式打印出来？
    //先把它转换到vector<uint8_t>
    std::vector<uint8_t> v1(sizeof(result));
    std::memcpy(v1.data(), &result, sizeof(result));

    cout << "最终结果: " << format_to_string(v1);
    return 0;
}