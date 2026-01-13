cmake_minimum_required(VERSION 3.22)
project(cmake_testapp)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options("/utf-8")
endif ()

# 下面三行用户控制最终输出的可执行文件所在路径，这里我放到项目根目录的dist目录下
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY output)  # 一般不使用这行，而是debug和release分开设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../dist/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../dist/release)

set(MY_SOURCES main.cpp general.cpp calc_lib.cpp MaduoRunner.cpp)
add_executable(exe1 ${MY_SOURCES})

add_executable(cmake_testapp main.cpp general.cpp)
add_executable(calc calc.cpp calc_lib.cpp)
add_library(cmake_testapp_lib STATIC calc_lib.cpp calc_lib.h)

target_include_directories(cmake_testapp_lib PUBLIC includes/general)
target_include_directories(cmake_testapp_lib PUBLIC includes/math)

find_library(TEST_LIBRARY cmake_testapp_lib lib)    # 在lib目录下找这个库文件
target_link_libraries(cmake_testapp LINK_PUBLIC ${TEST_LIBRARY})        # 链接静态库，target_link_libraries要在add_executable之后

# The enable_testing command creates a built-in target test which will execute CTest.
enable_testing()
add_subdirectory(ctests)
add_executable(adjacent_find 算法库adjacent_find.cpp)
add_executable(transform 算法库transform.cpp)
add_executable(cctype_demo 所有cctype函数.cpp)
add_executable(right_ref_demo 复杂的右值引用案例.cpp)

add_executable(struct_bit_protocol_demo struct封装协议.cpp)

add_executable(base_drived_demo 基类和派生类的析构函数调用顺序.cpp)
#此命令会查找 Boost 库，但它不会使用配置文件（config file）。它主要依赖于构建系统中的环境变量和已安装的库的位置。如果找不到 Boost 库，这将导致构建失败。
# COMPONENTS关键字用于指定要查找的库的组件。例如，如果你正在查找一个名为SomeLibrary的库，并且该库具有多个组件（例如，A、B、C等），
# 则可以使用COMPONENTS A B C来指定你希望CMake查找哪些组件。这有助于CMake确定需要查找哪些文件和路径，以及在找不到所需组件时提供更具体的错误消息。
find_package(Boost 1.83.0 REQUIRED COMPONENTS timer)

#如果里面有个CONFIG关键字，此命令会尝试通过读取 Boost 的配置文件（通常为 boostconfig.cmake 或 FindBoost.cmake）来查找 Boost 库。
#这种方式的优点是它可以自动找到 Boost 库的安装位置，而无需手动设置环境变量。
#这对于那些没有将 Boost 库的二进制文件放在标准位置的系统来说特别有用。
#如果无法找到 Boost 的配置文件，或者找到了但无法从中获取 Boost 的路径，那么这个命令也会导致构建失败。
#find_package(Boost CONFIG REQUIRED timer)
target_link_libraries(base_drived_demo PRIVATE Boost::timer)

find_package(absl CONFIG REQUIRED)
# target_link_libraries指令中有PRIVATE PUBLIC INTERFACE这三种用法，其中PRIVATE PUBLIC用的较多，INTERFACE不常用
# 为什么区分，我们这样来举例，假设lib1依赖于lib2，myexe可执行程序如果只依赖于lib1，那么就用lib1 PRIVATE lib2，lib2不会传递给myexe
# 如果myexe也依赖于lib2，那么就用lib1 PUBLIC lib2，lib2会自动传递给myexe，不需要写myexe PRIVATE lib1 lib2，只需要写myexe PRIVATE lib1，这就是依赖传递
# 如果lib1依赖lib2但是只在lib1的头文件中引用了lib2，没有在lib1的cpp文件中使用lib2，此时lib1不会链接到lib2，myexe依赖lib1后，会自动依赖到lib2，可以使用lib2中的功能
# 这种lib1 PUBLIC lib2不常用，很少，直接用lib1 PUBLIC lib2得了
# 一句话总结，如果lib1 PRIVATE lib2，那么依赖了lib1的target，不能自动链接到lib2，必须写：myexe PRIVATE lib1 lib2
# 如果lib1 PUBLIC lib2，那么依赖了lib1的target，会自动链接到lib2，只需要写：myexe PRIVATE lib1

# target_link_libraries(lib1 PRIVATE|PUBLIC|INTERFACE lib2)
# target_link_libraries(myexe PRIVATE lib1)

# 为什么下面出现absl::any 中间有::，是因为有些开源项目会编译多个库出来让用户可选使用，它的编译脚本中是这样给库取名字的，比如
# add_library(absl::log_internal_globals INTERFACE IMPORTED)    add_library的第一个参数就是库的名字，库名字里面本身就带了::
target_link_libraries(base_drived_demo PRIVATE absl::any absl::log absl::base absl::bits)

add_executable(soem_demo1 soem_demo1.cpp)

find_package(soem CONFIG REQUIRED)
target_link_libraries(soem_demo1 PRIVATE soem)

find_package(folly CONFIG REQUIRED)
#target_link_libraries(main PRIVATE Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util)
add_executable(cout_fmt_demo cout格式化输出.cpp)



add_executable(modbus_demo modbus测试.cpp)
#在 CMake 中，使用 find_package 找到库文件后，通常不再需要使用 target_link_directories()。
#find_package 用于搜索外部库，它会自动处理库的路径和链接问题。一旦找到了库，CMake 会将库的路径和相关信息存储在变量中，
#例如 FOO_INCLUDE_DIRS 和 FOO_LIBRARIES，其中 FOO 是你使用 find_package 搜索的库的名称。
#然后，你可以使用这些变量来设置你的目标链接库。例如，你可以在你的目标上使用 target_link_libraries() 来链接找到的库，
#target_link_libraries(YourTargetName ${FOO_LIBRARIES})
find_package(libmodbus CONFIG REQUIRED)
target_link_libraries(modbus_demo PRIVATE modbus)


add_executable(http_client_demo http测试.cpp)
find_package(cpprestsdk CONFIG REQUIRED)
target_link_libraries(http_client_demo PRIVATE cpprestsdk::cpprest cpprestsdk::cpprestsdk_zlib_internal cpprestsdk::cpprestsdk_brotli_internal)

add_executable(tcp_demo tcp测试.cpp)
target_include_directories(tcp_demo PUBLIC "D:\\CLionProjects\\.vcpkg-clion\\vcpkg\\installed\\x64-windows\\include")

add_executable(nested_class1 嵌套类测试.cpp)

add_executable(libuv_demo1 测试libuv.cpp)
find_package(libuv CONFIG REQUIRED)
target_link_libraries(libuv_demo1 PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)


add_executable(jemalloc_demo jemalloc测试.cpp)
target_include_directories(jemalloc_demo PUBLIC "D:\\CLionProjects\\.vcpkg-clion\\vcpkg\\installed\\x64-windows\\include")
target_link_directories(jemalloc_demo PUBLIC "D:\\CLionProjects\\.vcpkg-clion\\vcpkg\\installed\\x64-windows\\lib")
target_link_libraries(jemalloc_demo PRIVATE jemalloc)

#find_package(jemalloc REQUIRED)
#target_link_libraries(jemalloc_demo PRIVATE jemalloc)
add_executable(calc_hash_value 计算hash值.cpp)
find_package(Boost REQUIRED COMPONENTS container)
target_link_libraries(calc_hash_value PRIVATE Boost::container)

add_executable(usb_demo1 usb测试.cpp)
find_package(libusb CONFIG REQUIRED)
message(${LIBUSB_INCLUDE_DIRS})
message(${LIBUSB_LIBRARIES})
target_include_directories(usb_demo1 PRIVATE ${LIBUSB_INCLUDE_DIRS})
target_link_libraries(usb_demo1 PRIVATE ${LIBUSB_LIBRARIES})



add_executable(find_u_disk1 找硬盘.cpp)


add_executable(memory_allocator_demo1 内存Allocator演示.cpp)
target_include_directories(memory_allocator_demo1 PUBLIC "D:\\CLionProjects\\.vcpkg-clion\\vcpkg\\installed\\x64-windows\\include")

add_executable(boost_string_demo1 Boost库String的增强.cpp)
target_include_directories(boost_string_demo1 PUBLIC "D:\\CLionProjects\\.vcpkg-clion\\vcpkg\\installed\\x64-windows\\include")

add_executable(google_abseil_demo 谷歌abseil库的使用.cpp)
target_link_libraries(google_abseil_demo PRIVATE absl::abseil_dll)
add_executable(iterator_demo 标准库迭代器测试.cpp)

file(GLOB_RECURSE geom3d_sources LIST_DIRECTORIES false geom3d/*.cpp)
add_executable(test_geom3d ${geom3d_sources})

#
add_executable(poco_net_demo poconet测试.cpp)
find_package(Poco REQUIRED COMPONENTS Net)
target_link_libraries(poco_net_demo PRIVATE Poco::Net)


add_executable(zeromq_protobuf_demo zeromq和protobuf测算.cpp)
find_package(ZeroMQ CONFIG REQUIRED)
target_link_libraries(zeromq_protobuf_demo PRIVATE libzmq libzmq-static)

find_package(protobuf CONFIG REQUIRED)
target_link_libraries(zeromq_protobuf_demo PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)


# 测试zeromq这个网络通讯库
find_package(cppzmq CONFIG REQUIRED)

add_executable(zclient zeromq/zclient.cpp)
add_executable(zserver zeromq/zserver.cpp)
target_link_libraries(zclient PRIVATE libzmq libzmq-static)
target_link_libraries(zclient PRIVATE cppzmq cppzmq-static)

target_link_libraries(zserver PRIVATE libzmq libzmq-static)
target_link_libraries(zserver PRIVATE cppzmq cppzmq-static)