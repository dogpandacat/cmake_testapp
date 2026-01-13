//
// Created by AndyCui on 2023/12/20.
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main ()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, zmq::socket_type::req);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    char contents[] = "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长Hello这是你个待测试的数据，很长很长"
                      "！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！#";
    std::cout << "sizeof(contents) = " << sizeof(contents) << std::endl;    //这会输出6，因为C风格的字符串，编译器会自动在最后加一个'\0'
    std::cout << "strlen(contents) = " << strlen(contents) << std::endl;    //这会输出5，strlen函数会检查每个字符，遇到'\0'就会停止
    //  Do 10 requests, waiting each time for a response
    for (int request_nbr = 0; request_nbr != 100000; request_nbr++) {
        zmq::message_t request (sizeof(contents) - 1);
        memcpy (request.data (), contents, sizeof(contents) - 1 );
        std::cout << "Sending Hello " << request_nbr << "..." << std::endl;
        socket.send (request, zmq::send_flags::none);

        //  Get the reply.
        zmq::message_t reply;
        socket.recv (reply, zmq::recv_flags::none);
        std::cout << "Received World " << request_nbr << std::endl;

        std::this_thread::sleep_for(chrono::seconds(5));
    }
    return 0;
}