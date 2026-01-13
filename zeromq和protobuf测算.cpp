//
// Created by AndyCui on 2023/10/23.
//

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>

#include <zmq.h>

using namespace std;

void testProtobufUDP()
{

}

int testZeromq()
{
    cout << "*****************************" << endl;

    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    cout << "starting..." << endl;

    int rc = zmq_connect(socket, "tcp://localhost:5555");
    if (rc != 0) {
        cout << "Error: " << zmq_strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    char request[] = "abcdefg";
    cout << "1111111" << endl;
    rc = zmq_send(socket, request, strlen(request), 0);
    cout << "22222222" << endl;
    if (rc != strlen(request)) {
        cout << "Error: " << zmq_strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    cout << "333333" << endl;
    char buffer[1024];
    rc = zmq_recv(socket, buffer, sizeof(buffer), 0);
    if (rc == -1) {
        cout << "Error: " << zmq_strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Received response: " << buffer << endl;

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}

int main(int argc, char* argv[])
{
    testZeromq();
}