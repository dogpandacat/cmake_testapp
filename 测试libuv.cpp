#include <iostream>
#include <uv.h>
#include <thread>
#include <chrono>

using namespace std;

static void connect_cb(uv_connect_t* req, int status) {

}

static void on_read_alloc(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
    static char base[1024];

    buf->base = base;
    buf->len = sizeof(base);
}


static void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf) {
    if (nread < 0) {
        return;
    }

}

int test_connect()
{
    uv_loop_t *loop = uv_default_loop();

    struct sockaddr_in addr;
    uv_tcp_t client;
    uv_connect_t connect;
    uv_read_t read;
    uv_buf_t buf;

    // 创建 TCP 句柄
    uv_tcp_init(loop, &client);

    // 连接到服务端
    uv_ip4_addr("127.0.0.1", 8080, &addr);
    uv_tcp_connect(&connect, &client,(struct sockaddr*)&addr, connect_cb);

    // 开始读取数据
    buf = uv_buf_init((char*)malloc(1024), 1024);
    uv_read_start((uv_stream_t*)&client, on_read_alloc, on_read);

    // 运行事件循环
    uv_run(loop, UV_RUN_DEFAULT);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    return 0;
}

int64_t counter = 0;

void wait_for_a_while(uv_idle_t* handle) {
    counter++;
    if (counter >= 10e6)
        uv_idle_stop(handle);
}

void test_libuv_idle()
{
    uv_idle_t idler;

    uv_idle_init(uv_default_loop(), &idler);
    uv_idle_start(&idler, wait_for_a_while);

    printf("Idling...\n");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_loop_close(uv_default_loop());
}

int test_httpclient()
{


    return 0;
}

int main() {
//    test_connect();
//    test_libuv_idle();
    return 0;
}