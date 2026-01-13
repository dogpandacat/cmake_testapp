//
// Created by AndyCui on 2023/10/13.
//

#include <brynet/base/AppStatus.hpp>
#include <brynet/net/AsyncConnector.hpp>
#include <brynet/net/TcpService.hpp>
#include <brynet/net/wrapper/ConnectionBuilder.hpp>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

using namespace brynet;
using namespace brynet::net;
using namespace std;

#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <chrono>
#include <thread>

// 定义结构体
struct MyStruct {
    int id;
    char name[20];
};

int windows_tcp()
{
// 初始化WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // 创建TCP连接
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // 服务端监听的端口号
    serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // 服务端IP地址

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // 设置连接超时
    DWORD timeout = 5000; // 5 seconds
    if (setsockopt(sock, SOL_SOCKET, SO_CONNECT_TIMEOUT, (const char*)&timeout, sizeof(timeout)) != 0) {
        std::cerr << "setsockopt failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 连接服务器，带有超时机制
    if (connect(sock, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) != 0) {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            std::cerr << "Connection timed out." << std::endl;
        } else {
            std::cerr << "Connection failed." << std::endl;
        }
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 发送结构体数据
    MyStruct myStruct = {1, "hello"};
    send(sock, (char*)&myStruct, sizeof(myStruct), 0);

    // 接收服务器响应
    char response[1024];
    int bytesReceived = recv(sock, response, 1024, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 将接收到的字节流转换为结构体
    MyStruct receivedStruct;
    memcpy(&receivedStruct, response, bytesReceived);
    std::cout << "Received struct: id=" << receivedStruct.id << ", name=" << receivedStruct.name << std::endl;

    // 关闭连接并清理WinSock
    closesocket(sock);
    WSACleanup();

    return 0;
}

void test_brynet(int argc, char* argv[])
{
    if (argc != 6)
    {
        fprintf(stderr, "Usage: <host> <port> <net work thread num> <session num> <packet size> \n");
        exit(-1);
    }

    std::string tmp(atoi(argv[5]), 'a');

    auto service = IOThreadTcpService::Create();
    service->startWorkerThread(atoi(argv[3]));

    auto connector = AsyncConnector::Create();
    connector->startWorkerThread();

    auto enterCallback = [tmp](const TcpConnection::Ptr& session) {
        session->setDataCallback([session](brynet::base::BasePacketReader& reader) {
            session->send(reader.begin(), reader.size());
            reader.consumeAll();
        });
        session->send(tmp);
    };

    auto failedCallback = []() {
        std::cout << "connect failed" << std::endl;
    };

    wrapper::ConnectionBuilder connectionBuilder;
    connectionBuilder
            .WithService(service)
            .WithConnector(connector)
            .WithMaxRecvBufferSize(1024 * 1024)
            .AddEnterCallback(enterCallback);

    const auto num = std::atoi(argv[4]);
    const auto ip = argv[1];
    const auto port = std::atoi(argv[2]);
    for (auto i = 0; i < num; i++)
    {
        try
        {
            connectionBuilder
                    .WithAddr(ip, port)
                    .WithTimeout(std::chrono::seconds(2))   //连接2s超时
                    .WithFailedCallback(failedCallback)
                    .AddSocketProcessCallback([](TcpSocket& socket) {
                        cout << "socket process here!" << endl;
                        socket.setNodelay();
                    })
                    .asyncConnect();
        }
        catch (std::runtime_error& e)
        {
            std::cout << "error:" << e.what() << std::endl;
        }
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (brynet::base::app_kbhit())
        {
            break;
        }
    }
}

int main(int argc, char** argv)
{

    windows_tcp();
    return 0;
}