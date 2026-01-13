//
// Created by AndyCui on 2023/12/20.
//
#include <zmq.hpp>
#include <iostream>
#include <string>
#include <optional>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <cassert>
#define sleep(n)	Sleep(n)
#endif

using namespace std;

int main(int argc, char* argv[])
{
    //所谓的上下文，其实是一些参数，这里配置IO线程为2，最大可接受的客户端数量为2
    zmq::context_t context (2, 2);
    context.set(zmq::ctxopt::max_sockets, 2);
    zmq::socket_t server (context, zmq::socket_type::rep);
    server.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t message;
        try{
            //  Wait for next message from client，传none是阻塞，一直到客户端有数据过来
            std::optional<size_t> result = server.recv (message, zmq::recv_flags::none);
            if(result.has_value()) {
                string str(static_cast<char*>(message.data()), result.value());
                std::cout << "Received字节数：" << result.value() << ", 内容 = " << str <<  std::endl;
            }
            //  Do some 'work'
            sleep(1);
            //  Send reply back to client
            zmq::message_t reply (5);
            memcpy (reply.data (), "World", 5);
            server.send (reply, zmq::send_flags::none);
        } catch(zmq::error_t& e) {
            if (e.num() == EINTR)
                std::cout << "W: interrupted" << std::endl;
            else
                std::cout << "E: error, errnum = " << e.num() << ", what = " << e.what()
                          << std::endl;
            break;  // Interrupted
        }

    }
    return 0;
}
